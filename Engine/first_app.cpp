/*
#include "first_app.h"


#include "keyboard_movement_controller.h"
#include "lve_buffer.h"
#include "Camera/lve_camera.h"
#include "Systems/point_light_system.h"
#include "Systems/simple_render_system.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace lve
{
	// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	FirstApp::FirstApp()
	{
		LveDescriptorPool::Builder builder{lveDevice};
		builder.SetMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		       .AddPoolSize(vk::DescriptorType::eUniformBuffer, LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		globalPool = builder.Build();
		LoadGameObjects();
	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::Run()
	{
		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible);

			uboBuffers[i]->Map();
		}

		auto global_set_layout = LveDescriptorSetLayout::Builder(lveDevice)
		                         .AddBinding(0, vk::DescriptorType::eUniformBuffer,
		                                     vk::ShaderStageFlagBits::eAllGraphics)
		                         .Build();

		std::vector<vk::DescriptorSet> global_descriptor_sets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < global_descriptor_sets.size(); i++)
		{
			auto buffer_info = uboBuffers[i]->DescriptorInfo();
			LveDescriptorWriter(*global_set_layout, *globalPool)
				.WriteBuffer(0, &buffer_info)
				.Build(global_descriptor_sets[i]);
		}

		SimpleRenderSystem simple_render_system{
			lveDevice, lveRenderer.GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
		};
		PointLightSystem point_light_system{
			lveDevice, lveRenderer.GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
		};
		LveCamera camera{};

		auto viewer_object                    = LveGameObject::CreateGameObject();
		viewer_object.transform.translation.z = -2.5f;
		KeyboardMovementController camera_controller{};


		auto current_time = std::chrono::high_resolution_clock::now();

		while (!lveWindow.ShouldClose())
		{
			glfwPollEvents();

			auto  new_time   = std::chrono::high_resolution_clock::now(); // Bien mettre après la gestion d'event
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).
				count();
			current_time = new_time;

			camera_controller.MoveInPlaneXZ(lveWindow.GetGlfwWindow(), frame_time, viewer_object);
			camera.SetViewYXZ(viewer_object.transform.translation, viewer_object.transform.rotation);

			float aspect = lveRenderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto command_buffer = lveRenderer.BeginFrame())
			{
				int frame_index = lveRenderer.GetFrameIndex();

				FrameInfo frame_info{
					frame_index,
					frame_time,
					command_buffer,
					camera,
					global_descriptor_sets[frame_index],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projection  = camera.GetProjection();
				ubo.view        = camera.GetView();
				ubo.inverseView = camera.GetInverseView();

				uboBuffers[frame_index]->WriteToBuffer(&ubo);
				uboBuffers[frame_index]->Flush();

				// render
				lveRenderer.BeginSwapChainRenderPass(command_buffer); //begin offscreen shadow pass

				// order here matters
			             //render shadow casting objects


				lveRenderer.EndSwapChainRenderPass(command_buffer);
				lveRenderer.EndFrame(); //end offscreen shadow pass
			}
		}

		lveDevice.Device().waitIdle();
	}


	void FirstApp::LoadGameObjects()
	{
		std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(lveDevice, "Models\\flat_vase.obj");

		auto flat_vase_go                  = LveGameObject::CreateGameObject();
		flat_vase_go.model                 = lve_model;
		flat_vase_go.transform.translation = {-.5f, .5f, 0.f};
		flat_vase_go.transform.scale       = {3.f, 1.5f, 3.f};
		gameObjects.emplace(flat_vase_go.GetId(), std::move(flat_vase_go));

		lve_model                            = LveModel::CreateModelFromFile(lveDevice, "Models\\smooth_vase.obj");
		auto smooth_vase_go                  = LveGameObject::CreateGameObject();
		smooth_vase_go.model                 = lve_model;
		smooth_vase_go.transform.translation = {.5f, .5f, 0.f};
		smooth_vase_go.transform.scale       = {3.f, 1.5f, 3.f};
		gameObjects.emplace(smooth_vase_go.GetId(), std::move(smooth_vase_go));

		lve_model                     = LveModel::CreateModelFromFile(lveDevice, "Models\\quad.obj");
		auto quad_go                  = LveGameObject::CreateGameObject();
		quad_go.model                 = lve_model;
		quad_go.transform.translation = {.0f, .5f, 0.f};
		quad_go.transform.scale       = {3.f, 1.f, 3.f};
		gameObjects.emplace(quad_go.GetId(), std::move(quad_go));

		lve_model                    = LveModel::CreateModelFromFile(lveDevice, "Models\\viking_room.obj");
		auto viking                  = LveGameObject::CreateGameObject();
		viking.model                 = lve_model;
		viking.transform.translation = {0.f, 0.f, 5.f};
		viking.transform.scale       = {3.f, 3.f, 3.f};
		viking.transform.rotation    = {glm::radians(90.0f), glm::radians(90.0f), 0.0f};
		gameObjects.emplace(viking.GetId(), std::move(viking));

		std::vector<glm::vec3> light_colors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f} //
		};

		for (int i = 0; i < light_colors.size(); i++)
		{
			auto point_light  = LveGameObject::MakePointLight(0.2f);
			point_light.color = light_colors[i];
			auto rotate_light = rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / light_colors.size(),
				{0.f, -1.f, 0.f});
			point_light.transform.translation = glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(point_light.GetId(), std::move(point_light));
		}
	}
} // namespace lve
*/
