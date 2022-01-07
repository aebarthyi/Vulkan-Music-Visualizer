#include "vmv_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace vmv {

    // *************** Descriptor Set Layout Builder *********************

    vmvDescriptorSetLayout::Builder& vmvDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<vmvDescriptorSetLayout> vmvDescriptorSetLayout::Builder::build() const {
        return std::make_unique<vmvDescriptorSetLayout>(vmv_Device, bindings);
    }

    // *************** Descriptor Set Layout *********************

    vmvDescriptorSetLayout::vmvDescriptorSetLayout(
        vmvDevice& vmv_Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : vmv_Device{ vmv_Device }, bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            vmv_Device.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    vmvDescriptorSetLayout::~vmvDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(vmv_Device.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    vmvDescriptorPool::Builder& vmvDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    vmvDescriptorPool::Builder& vmvDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }
    vmvDescriptorPool::Builder& vmvDescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<vmvDescriptorPool> vmvDescriptorPool::Builder::build() const {
        return std::make_unique<vmvDescriptorPool>(vmv_Device, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    vmvDescriptorPool::vmvDescriptorPool(
        vmvDevice& vmv_Device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : vmv_Device{ vmv_Device } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(vmv_Device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    vmvDescriptorPool::~vmvDescriptorPool() {
        vkDestroyDescriptorPool(vmv_Device.device(), descriptorPool, nullptr);
    }

    bool vmvDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(vmv_Device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void vmvDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            vmv_Device.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void vmvDescriptorPool::resetPool() {
        vkResetDescriptorPool(vmv_Device.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    vmvDescriptorWriter::vmvDescriptorWriter(vmvDescriptorSetLayout& setLayout, vmvDescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    vmvDescriptorWriter& vmvDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    vmvDescriptorWriter& vmvDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool vmvDescriptorWriter::build(VkDescriptorSet& set) {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void vmvDescriptorWriter::overwrite(VkDescriptorSet& set) {
        for (auto& write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.vmv_Device.device(), writes.size(), writes.data(), 0, nullptr);
    }

}  // namespace vmv