#include "first_app.h"

#include "simple_render_system.h"   
#include "lve_camera.h"   
#include "keyboard_movement_controller.h"   

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace lve {

    FirstApp::FirstApp() {
        LoadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::Run() {
        SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.GetSwapChainRenderPass() };
        LveCamera camera{};
        //camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        
        auto viewerObject = LveGameObject::CreateGameObject();
        KeyboardMovementController cameraController{};


        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.ShouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now(); // Bien mettre après la gestion d'event
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(lveWindow.GetGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.GetAspectRatio();
            //camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                //begin offscreen shadow pass
                //render shadow casting objects
                //end offscreen shadow pass

                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }
    

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<LveModel> lveModel = LveModel::CreateModelFromFile(lveDevice, "Models\\smooth_vase.obj");

        auto gameObject = LveGameObject::CreateGameObject();
        gameObject.model = lveModel;
        gameObject.transform.translation = { .0f, .0f, 2.5f };
        gameObject.transform.scale = glm::vec3{ 3.f };
        gameObjects.push_back(std::move(gameObject));
    }
}  // namespace lve