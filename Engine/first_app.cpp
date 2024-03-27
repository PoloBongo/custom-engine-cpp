#include "first_app.h"


#include "lve_camera.h"   
#include "keyboard_movement_controller.h"   
#include "lve_buffer.h"   
#include "Systems/simple_render_system.h"   
#include "Systems/point_light_system.h"  

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <iostream>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <numeric>

namespace lve {

    FirstApp::FirstApp() {
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .SetMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();
        LoadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::Run() {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            uboBuffers[i]->map();
        }

        auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .Build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()};
        PointLightSystem pointLightSystem{ lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()};
        LveCamera camera{};
        
        auto viewerObject = LveGameObject::CreateGameObject();
        viewerObject.transform.translation.z = -2.5f;
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
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                int frameIndex = lveRenderer.GetFrameIndex();
                
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.GetProjection();
                ubo.view = camera.GetView();
                ubo.inverseView = camera.GetInverseView();
                pointLightSystem.Update(frameInfo, ubo);

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);//begin offscreen shadow pass

                // order here matters
                simpleRenderSystem.RenderGameObjects(frameInfo);//render shadow casting objects
                pointLightSystem.Render(frameInfo);//render shadow casting objects


                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();//end offscreen shadow pass
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }
    

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<LveModel> lveModel = LveModel::CreateModelFromFile(lveDevice, "Models\\flat_vase.obj");

        auto flatVaseGO = LveGameObject::CreateGameObject();
        flatVaseGO.model = lveModel;
        flatVaseGO.transform.translation = { -.5f, .5f, 0.f };
        flatVaseGO.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.emplace(flatVaseGO.GetId(), std::move(flatVaseGO));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "Models\\smooth_vase.obj");
        auto smoothVaseGO = LveGameObject::CreateGameObject();
        smoothVaseGO.model = lveModel;
        smoothVaseGO.transform.translation = { .5f, .5f, 0.f };
        smoothVaseGO.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.emplace(smoothVaseGO.GetId(), std::move(smoothVaseGO));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "Models\\quad.obj");
        auto quadGO = LveGameObject::CreateGameObject();
        quadGO.model = lveModel;
        quadGO.transform.translation = { .0f, .5f, 0.f };
        quadGO.transform.scale = { 3.f, 1.f, 3.f };
        gameObjects.emplace(quadGO.GetId(), std::move(quadGO));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "Models\\viking_room.obj");
        auto Viking = LveGameObject::CreateGameObject();
        Viking.model = lveModel;
        Viking.transform.translation = { 0.f, 0.f, 5.f };
        Viking.transform.scale = { 3.f, 3.f, 3.f };
        Viking.transform.rotation = { glm::radians(90.0f), glm::radians(90.0f), 0.0f };
        gameObjects.emplace(Viking.GetId(), std::move(Viking));

        std::vector<glm::vec3> lightColors{
    {1.f, .1f, .1f},
    {.1f, .1f, 1.f},
    {.1f, 1.f, .1f},
    {1.f, 1.f, .1f},
    {.1f, 1.f, 1.f},
    {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = LveGameObject::MakePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                { 0.f, -1.f, 0.f });
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
        }

    }
}  // namespace lve