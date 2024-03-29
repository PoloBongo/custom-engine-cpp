#include "CoreEngine.h"
#include "Managers/WindowManager.h"

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
#include <numeric>

void WindowManager::Init() {
	Module::Init();

	builder.SetMaxSets(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder.Build();

	gameObjects = new lve::LveGameObject::Map;
	camera = new lve::LveCamera{};
	LoadGameObjects();
}

void WindowManager::Start()
{
	Module::Start();

	uboBuffers.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<lve::LveBuffer>(
			lveDevice,
			sizeof(lve::GlobalUbo),
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible);

		uboBuffers[i]->map();
	}

	auto globalSetLayout = lve::LveDescriptorSetLayout::Builder(lveDevice)
		.AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eAllGraphics)
		.Build();

	globalDescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < globalDescriptorSets.size(); i++) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		lve::LveDescriptorWriter(*globalSetLayout, *globalPool)
			.WriteBuffer(0, &bufferInfo)
			.Build(globalDescriptorSets[i]);
	}

	simpleRenderSystem = new lve::SimpleRenderSystem{ lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };
	pointLightSystem = new lve::PointLightSystem{ lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };

	viewerObject = new lve::LveGameObject(lve::LveGameObject::CreateGameObject());
	viewerObject->transform.translation.z = -2.5f;

	currentTime = std::chrono::high_resolution_clock::now();
}

void WindowManager::FixedUpdate()
{
	Module::FixedUpdate();
}

void WindowManager::Update()
{
	Module::Update();

	if (!lveWindow.ShouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now(); // Bien mettre après la gestion d'event
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.MoveInPlaneXZ(lveWindow.GetGLFWwindow(), frameTime, *viewerObject);
		camera->SetViewYXZ(viewerObject->transform.translation, viewerObject->transform.rotation);

		float aspect = lveRenderer.GetAspectRatio();
		camera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = lveRenderer.BeginFrame()) {
			int frameIndex = lveRenderer.GetFrameIndex();

			lve::FrameInfo frameInfo{
				frameIndex,
				frameTime,
				commandBuffer,
				*camera,
				globalDescriptorSets[frameIndex],
				*gameObjects
			};

			// update
			lve::GlobalUbo ubo{};
			ubo.projection = camera->GetProjection();
			ubo.view = camera->GetView();

			pointLightSystem->Update(frameInfo, ubo);

			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			lveRenderer.BeginSwapChainRenderPass(commandBuffer);//begin offscreen shadow pass
			simpleRenderSystem->RenderGameObjects(frameInfo);//render shadow casting objects
			pointLightSystem->Render(frameInfo);//render shadow casting objects
			lveRenderer.EndSwapChainRenderPass(commandBuffer);
			lveRenderer.EndFrame();//end offscreen shadow pass
		}
	}
	else {
		vkDeviceWaitIdle(lveDevice.device());
		Engine::GetInstance()->Quit();
	}

}


void WindowManager::LoadGameObjects() {
	std::shared_ptr<lve::LveModel> lveModel = lve::LveModel::CreateModelFromFile(lveDevice, "Models\\flat_vase.obj");

	auto flatVaseGO = lve::LveGameObject::CreateGameObject();
	flatVaseGO.model = lveModel;
	flatVaseGO.transform.translation = { -.5f, .5f, 0.f };
	flatVaseGO.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects->emplace(flatVaseGO.GetId(), std::move(flatVaseGO));

	lveModel = lve::LveModel::CreateModelFromFile(lveDevice, "Models\\smooth_vase.obj");
	auto smoothVaseGO = lve::LveGameObject::CreateGameObject();
	smoothVaseGO.model = lveModel;
	smoothVaseGO.transform.translation = { .5f, .5f, 0.f };
	smoothVaseGO.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects->emplace(smoothVaseGO.GetId(), std::move(smoothVaseGO));

	lveModel = lve::LveModel::CreateModelFromFile(lveDevice, "Models\\quad.obj");
	auto quadGO = lve::LveGameObject::CreateGameObject();
	quadGO.model = lveModel;
	quadGO.transform.translation = { .0f, .5f, 0.f };
	quadGO.transform.scale = { 3.f, 1.f, 3.f };
	gameObjects->emplace(quadGO.GetId(), std::move(quadGO));

	lveModel = lve::LveModel::CreateModelFromFile(lveDevice, "Models\\viking_room.obj");
	auto Viking = lve::LveGameObject::CreateGameObject();
	Viking.model = lveModel;
	Viking.transform.translation = { 0.f, 0.f, 5.f };
	Viking.transform.scale = { 3.f, 3.f, 3.f };
	Viking.transform.rotation = { glm::radians(90.0f), glm::radians(90.0f), 0.0f };
	gameObjects->emplace(Viking.GetId(), std::move(Viking));

	std::vector<glm::vec3> lightColors{
{1.f, .1f, .1f},
{.1f, .1f, 1.f},
{.1f, 1.f, .1f},
{1.f, 1.f, .1f},
{.1f, 1.f, 1.f},
{1.f, 1.f, 1.f}  //
	};

	for (int i = 0; i < lightColors.size(); i++) {
		auto pointLight = lve::LveGameObject::MakePointLight(0.2f);
		pointLight.color = lightColors[i];
		auto rotateLight = glm::rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / lightColors.size(),
			{ 0.f, -1.f, 0.f });
		pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
		gameObjects->emplace(pointLight.GetId(), std::move(pointLight));
	}

}

void WindowManager::PreRender()
{
	Module::PreRender();

	//window->clear(sf::Color::Black);
}

void WindowManager::Render()
{
	Module::Render();
}

void WindowManager::RenderGui()
{
	Module::RenderGui();
}

void WindowManager::PostRender()
{
	Module::PostRender();

	//window->display();
}

void WindowManager::Release()
{
	Module::Release();

	//window->close();
}