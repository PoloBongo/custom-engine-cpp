#include "Modules/WindowModule.h"
#include "CoreEngine.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <numeric>
#include <stdexcept>

#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"

void WindowModule::Init()
{
	Module::Init();

	lveWindow = new lve::LveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; /// Fenêtre de l'application.
	lveDevice = new lve::LveDevice{ *lveWindow };
	lveRenderer = new lve::LveRenderer{ *lveWindow, *lveDevice };

	builder = new lve::LveDescriptorPool::Builder{ *lveDevice };
	builder->SetMaxSets(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
	.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();

	camera      = new lve::LveCamera{};
	LoadGameObjects();
}

void WindowModule::Start()
{
	Module::Start();

	uboBuffers.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<lve::LveBuffer>(
			*lveDevice,
			sizeof(lve::GlobalUbo),
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible);

		uboBuffers[i]->Map();
	}

	const auto global_set_layout = lve::LveDescriptorSetLayout::Builder(*lveDevice)
	                               .AddBinding(0, vk::DescriptorType::eUniformBuffer,
	                                           vk::ShaderStageFlagBits::eAllGraphics)
	                               .Build();

	globalDescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto buffer_info = uboBuffers[i]->DescriptorInfo();
		lve::LveDescriptorWriter(*global_set_layout, *globalPool)
			.WriteBuffer(0, &buffer_info)
			.Build(globalDescriptorSets[i]);
	}

	simpleRenderSystem = new lve::SimpleRenderSystem{
		*lveDevice, lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};
	pointLightSystem = new lve::PointLightSystem{
		*lveDevice, lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};

	viewerObject                          = new GameObject(GameObject::CreateGameObject());
	viewerObject->GetTransform()->SetPosition({ 0.f,0.f,-2.5f });
}

void WindowModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void WindowModule::Update()
{
	Module::Update();

	if (!lveWindow->ShouldClose())
	{
		glfwPollEvents();

		cameraController.MoveInPlaneXZ(lveWindow->GetGlfwWindow(), TimeModule::GetDeltaTime(), *viewerObject);
		camera->SetViewYXZ(viewerObject->GetPosition(), viewerObject->GetRotation());

		const float aspect = lveRenderer->GetAspectRatio();
		camera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		ubo.projection = camera->GetProjection();
		ubo.view = camera->GetView();

		pointLightSystem->Update(gameObjects, ubo);
	}
	else
	{
		lveDevice->Device().waitIdle();
		Engine::GetInstance()->Quit();
	}
}

void WindowModule::PreRender()
{
	Module::PreRender();
	p_commandBuffer = moduleManager->GetModule<RHIVulkanModule>()->GetCurrentCommandBuffer();
	if (p_commandBuffer)
	{
		frameIndex = lveRenderer->GetFrameIndex();
		const float frame_time = TimeModule::GetDeltaTime();
		// update
	
		uboBuffers[frameIndex]->WriteToBuffer(&ubo);
		uboBuffers[frameIndex]->Flush();
	}
}

void WindowModule::Render()
{
	Module::Render();
	simpleRenderSystem->RenderGameObjects(gameObjects, *camera, *p_commandBuffer, globalDescriptorSets[frameIndex]);      //render shadow casting objects
	pointLightSystem->Render(gameObjects, *camera, *p_commandBuffer, globalDescriptorSets[frameIndex]);                 //render shadow casting objects
	
}

void WindowModule::RenderGui()
{
	Module::RenderGui();
}

void WindowModule::PostRender()
{
	Module::PostRender();

	//window->display();
}

void WindowModule::Release()
{
	Module::Release();

	//window->close();
}
void WindowModule::Finalize()
{
	Module::Finalize();

	//window->close();
}

void WindowModule::LoadGameObjects()
{
	std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*lveDevice, "Models\\flat_vase.obj");

	auto flatVaseGO                  = GameObject::CreatePGameObject();
	flatVaseGO->model                 = lve_model;
	flatVaseGO->GetTransform()->SetPosition(glm::vec3{-.5f, .5f, 0.f});
	flatVaseGO->GetTransform()->SetScale(glm::vec3{3.f, 1.5f, 3.f});
	gameObjects.push_back(flatVaseGO);

	lve_model                            = lve::LveModel::CreateModelFromFile(*lveDevice, "Models\\smooth_vase.obj");
	auto smooth_vase_go                  = GameObject::CreatePGameObject();
	smooth_vase_go->model                 = lve_model;
	smooth_vase_go->GetTransform()->SetPosition(glm::vec3{ .5f, .5f, 0.f});
	smooth_vase_go->GetTransform()->SetScale(glm::vec3{3.f, 1.5f, 3.f});
	gameObjects.push_back(smooth_vase_go);

	auto quad_go = lve::PlaneGameObject::Creates(*lveDevice, glm::vec3{.0f, .5f, 0.f}, glm::vec3{3.f, 1.f, 3.f});
	gameObjects.push_back(quad_go);

	lve_model                    = lve::LveModel::CreateModelFromFile(*lveDevice, "Models\\viking_room.obj");
	auto viking                  = GameObject::CreatePGameObject();
	viking->model                 = lve_model;
	viking->GetTransform()->SetPosition(glm::vec3{0.f, 0.f, 5.f});
	viking->GetTransform()->SetScale(glm::vec3{3.f, 3.f, 3.f});
	viking->GetTransform()->SetRotation(glm::vec3{glm::radians(90.0f), glm::radians(90.0f), 0.0f});
	gameObjects.push_back(viking);

	auto cube = lve::CubeGameObject::Creates(*lveDevice);
	gameObjects.push_back(cube);

	auto color_cube = lve::CubeGameObject::CreateColors(*lveDevice, glm::vec3{0.f, 0.f, 10.f});
	gameObjects.push_back(color_cube);

	std::vector<glm::vec3> light_colors{
		{1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		{1.f, 1.f, 1.f}
	};

	for (int i = 0; i < light_colors.size(); i++)
	{
		auto point_light  = lve::LightGameObject::Creates(0.2f, 0.1f);
		point_light->color = light_colors[i];
		auto rotate_light = rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / light_colors.size(),
			{0.f, -1.f, 0.f});
		point_light->GetTransform()->SetPosition(glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f)));
		gameObjects.push_back(point_light);
	}

	auto sun = lve::LightGameObject::Creates(1000000.f, 2.0f, glm::vec3{0.f, -1000.f, 0.f});
	gameObjects.push_back(sun);
}

//void WindowModule::PreRender()
//{
//	Module::PreRender();
//
//	//window->clear(sf::Color::Black);
//}
//
//void WindowModule::Render()
//{
//	Module::Render();
//}
//
//void WindowModule::RenderGui()
//{
//	Module::RenderGui();
//}
//
//void WindowModule::PostRender()
//{
//	Module::PostRender();
//
//	//window->display();
//}
//
//void WindowModule::Release()
//{
//	Module::Release();
//
//	//window->close();
//}
