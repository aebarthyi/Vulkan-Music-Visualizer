#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "movement.hpp"
#include "vmv_camera.hpp"
#include "test_ui.hpp"

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

    FirstApp::FirstApp() { loadGameObjects(); }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {

        vmvTestUI testUI{
            vmv_Window,
            vmv_Device,
            vmv_Renderer.getSwapChainRenderPass(),
            vmv_Renderer.getImageCount()
        };

        SimpleRenderSystem simpleRenderSystem{ vmv_Device, vmv_Renderer.getSwapChainRenderPass() };

        vmvCamera camera{};

        static float fov = 50.f;
        static glm::vec3 scale = glm::vec3(3.f, 1.5f, 3.f);
        static float rotation = 0.0f;
        static float sensitivity = 1.0f;

        glfwSetInputMode(vmv_Window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        auto viewerObject = vmvGameObject::createGameObject();
        MovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!vmv_Window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            gameObjects[0].transform.scale = scale;
            gameObjects[0].transform.rotation.y = glm::radians(rotation);

            float aspect = vmv_Renderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(fov), aspect, 0.1f, 10.f);

            if (auto commandBuffer = vmv_Renderer.beginFrame()) {
                testUI.newFrame();
                vmv_Renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

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
        auto gameObj = vmvGameObject::createGameObject();
        gameObj.model = vmv_Model;
        gameObj.transform.translation = { .0f, .5f, 3.5f };
        gameObj.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
        gameObjects.push_back(std::move(gameObj));
    }

}  // namespace vmv