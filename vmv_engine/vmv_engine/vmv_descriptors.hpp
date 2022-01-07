#pragma once

#include "vmv_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vmv {

    class vmvDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(vmvDevice& vmv_Device) : vmv_Device{ vmv_Device } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<vmvDescriptorSetLayout> build() const;

        private:
            vmvDevice& vmv_Device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        vmvDescriptorSetLayout(
            vmvDevice& vmv_Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~vmvDescriptorSetLayout();
        vmvDescriptorSetLayout(const vmvDescriptorSetLayout&) = delete;
        vmvDescriptorSetLayout& operator=(const vmvDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        vmvDevice& vmv_Device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class vmvDescriptorWriter;
    };

   class vmvDescriptorPool {
    public:
        class Builder {
        public:
            Builder(vmvDevice& vmv_Device) : vmv_Device{ vmv_Device } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<vmvDescriptorPool> build() const;

        private:
            vmvDevice& vmv_Device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        vmvDescriptorPool(
            vmvDevice& vmv_Device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~vmvDescriptorPool();
        vmvDescriptorPool(const vmvDescriptorPool&) = delete;
        vmvDescriptorPool& operator=(const vmvDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        vmvDevice& vmv_Device;
        VkDescriptorPool descriptorPool;

        friend class vmvDescriptorWriter;
    };

    class vmvDescriptorWriter {
    public:
        vmvDescriptorWriter(vmvDescriptorSetLayout& setLayout, vmvDescriptorPool& pool);

        vmvDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        vmvDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        vmvDescriptorSetLayout& setLayout;
        vmvDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace vmv