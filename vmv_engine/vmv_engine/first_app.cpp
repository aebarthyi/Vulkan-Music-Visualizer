#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "movement.hpp"
#include "vmv_camera.hpp"
#include "test_ui.hpp"
#include "vmv_buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>
#include <chrono>
#include <filesystem>

namespace vmv {

    struct GlobalUbo {
        glm::mat4 projectionView{ 1.f };
        glm::vec4 ambientLightColors{ 1.f,1.f,1.f,0.2f };
        glm::vec3 lightPosition{ -1.f };
        alignas(16) glm::vec4 lightColo{ 1.f };

    };

    FirstApp::FirstApp() { 
        globalPool = vmvDescriptorPool::Builder(vmv_Device)
            .setMaxSets(vmvSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, vmvSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects(); 
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        std::vector<std::unique_ptr<vmvBuffer>> uboBuffers(vmvSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<vmvBuffer>(
                vmv_Device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = vmvDescriptorSetLayout::Builder(vmv_Device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(vmvSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            vmvDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        vmvTestUI testUI{
            vmv_Window,
            vmv_Device,
            vmv_Renderer.getSwapChainRenderPass(),
            vmv_Renderer.getImageCount()
        };

        SimpleRenderSystem simpleRenderSystem{ vmv_Device, vmv_Renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

        vmvCamera camera{};

        static float fov = 50.f;
        static glm::vec3 scale = glm::vec3(3.f, 1.5f, 3.f);
        static float rotation = 0.0f;
        static float sensitivity = 1.0f;

        glfwSetInputMode(vmv_Window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        auto viewerObject = vmvGameObject::createGameObject(); 
        viewerObject.transform.translation.z = -2.5f;
        MovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!vmv_Window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            gameObjects[0].transform.scale = scale;
            gameObjects[0].transform.rotation.x = glm::radians(rotation);

            float aspect = vmv_Renderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(fov), aspect, 0.1f, 100.f);

            if (auto commandBuffer = vmv_Renderer.beginFrame()) {
                int frameIndex = vmv_Renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                testUI.newFrame();
                vmv_Renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);

                {
                    ImGui::Begin("TEST_UI");
                    ImGui::Text(
                        "Some primitive UI elements.");

                    ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);
                    ImGui::SliderFloat("SCALE - Y", &scale.y, 0.0f, 5.0f);
                    ImGui::SliderFloat("X-AXIS ROTATION", &rotation, 0.0f, 360.0f);
                    ImGui::SliderFloat("MOUSE SENSITIVITY", &sensitivity, 0.0f, 5.0f);

                    ImGui::Text(
                        "Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate
                    );

                    if (glfwGetKey(vmv_Window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) ImGui::SetWindowFocus();

                    if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered()) {
                        glfwSetInputMode(vmv_Window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    }
                    else {
                        glfwSetInputMode(vmv_Window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        cameraController.moveMouse(vmv_Window.getWindow(), frameTime, viewerObject, sensitivity);
                    }

                    cameraController.moveKeyboard(vmv_Window.getWindow(), frameTime, viewerObject);
                    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
                    ImGui::End();
                }

                testUI.render(commandBuffer);

                vmv_Renderer.endSwapChainRenderPass(commandBuffer);
                vmv_Renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(vmv_Device.device());
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<vmvModel> vmv_Model = vmvModel::createModelFromFile(vmv_Device, "models/smooth_vase.obj");
        auto vase = vmvGameObject::createGameObject();
        vase.model = vmv_Model;
        vase.transform.translation = { .5f, .5f, .0f };
        vase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
        gameObjects.push_back(std::move(vase));

        vmv_Model = vmvModel::createModelFromFile(vmv_Device, "models/quad.obj");
        auto floor = vmvGameObject::createGameObject();
        floor.model = vmv_Model;
        floor.transform.translation = { .0f, .5f, .0f };
        floor.transform.scale = glm::vec3(3.f, 1.f, 3.f);
        gameObjects.push_back(std::move(floor));
    }

}  // namespace vmv