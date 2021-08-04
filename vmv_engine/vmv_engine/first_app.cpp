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
        static float scale = 1.f;
        static float rotation = 0.0f;

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
            
          
            cameraController.moveInPlaneXZ(vmv_Window.getWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            gameObjects[0].transform.scale = glm::vec3(scale);
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
                    ImGui::SliderFloat("SCALE", &scale, 0.0f, 5.0f);
                    ImGui::SliderFloat("X-AXIS ROTATION", &rotation, 0.0f, 360.0f);

                    ImGui::Text(
                        "Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate
                    );

                    ImGui::ShowDemoWindow();
                    ImGui::End();
                }



                testUI.render(commandBuffer);


                vmv_Renderer.endSwapChainRenderPass(commandBuffer);
                vmv_Renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(vmv_Device.device());
    }

    std::unique_ptr<vmvModel> createCubeModel(vmvDevice& device, glm::vec3 offset) {
        vmvModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

        return std::make_unique<vmvModel>(device, modelBuilder);
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<vmvModel> vmv_Model = vmvModel::createModelFromFile(vmv_Device, "C:\\Users\\aebar\\Documents\\GitHub\\Vulkan-Music-Visualizer\\vmv_engine\\vmv_engine\\models\\colored_cube.obj");
        auto gameObj = vmvGameObject::createGameObject();
        gameObj.model = vmv_Model;
        gameObj.transform.translation = { .0f, .0f, 3.5f };
        gameObj.transform.scale = glm::vec3(1.f);
        gameObjects.push_back(std::move(gameObj));
    }

}  // namespace vmv