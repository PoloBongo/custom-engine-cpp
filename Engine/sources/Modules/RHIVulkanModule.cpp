#include "Modules/RHIVulkanModule.h"

#include "ModuleManager.h"
#include "Transform.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"

RHIVulkanModule::RHIVulkanModule()
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}
void RHIVulkanModule:: Init()
{
	windowModule = moduleManager->GetModule<WindowModule>();
	p_lveDevice = new lve::LveDevice{ windowModule };
	p_lveRenderer = new lve::LveRenderer{ windowModule, *p_lveDevice };
}

void RHIVulkanModule::Start()
{
	builder = new lve::LveDescriptorPool::Builder{ *p_lveDevice };
	builder->SetMaxSets(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();

	camera = new lve::LveCamera{};
	uboBuffers.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<lve::LveBuffer>(
			*p_lveDevice,
			sizeof(lve::GlobalUbo),
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible);

		uboBuffers[i]->Map();
	}

	const auto global_set_layout = lve::LveDescriptorSetLayout::Builder(*p_lveDevice)
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
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};
	pointLightSystem = new lve::PointLightSystem{
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};

	viewerObject = new GameObject(GameObject::CreateGameObject());
	viewerObject->GetTransform()->SetPosition(glm::vec3{ 0.f,0.f,-2.5f });

}

void RHIVulkanModule::FixedUpdate()
{
}

void RHIVulkanModule::Update()
{
	gameObjects = moduleManager->GetModule<SceneManager>()->GetCurrentScene()->GetAllGameObject();

	cameraController.MoveInPlaneXZ(windowModule->GetGlfwWindow(), TimeModule::GetDeltaTime(), *viewerObject);
	camera->SetViewYXZ(viewerObject->GetPosition(), viewerObject->GetRotation());

	const float aspect = p_lveRenderer->GetAspectRatio();
	camera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

	ubo.projection = camera->GetProjection();
	ubo.view = camera->GetView();

	pointLightSystem->Update(gameObjects, ubo);
}

void RHIVulkanModule::PreRender()
{
	currentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
	if (currentCommandBuffer)
	{
		frameIndex = p_lveRenderer->GetFrameIndex();
		const float frame_time = TimeModule::GetDeltaTime();
		// update

		uboBuffers[frameIndex]->WriteToBuffer(&ubo);
		uboBuffers[frameIndex]->Flush();
		p_lveRenderer->BeginSwapChainRenderPass(*currentCommandBuffer);
	}
	
}

void RHIVulkanModule::Render()
{
	simpleRenderSystem->RenderGameObjects(gameObjects, *camera, *currentCommandBuffer, globalDescriptorSets[frameIndex]);      //render shadow casting objects
	pointLightSystem->Render(gameObjects, *camera, *currentCommandBuffer, globalDescriptorSets[frameIndex]);                 //render shadow casting objects
}

void RHIVulkanModule::RenderGui()
{
}

void RHIVulkanModule::PostRender()
{
	p_lveRenderer->EndSwapChainRenderPass(*currentCommandBuffer);
	p_lveRenderer->EndFrame(currentCommandBuffer.get());
}

void RHIVulkanModule::Release()
{
}

void RHIVulkanModule::Finalize()
{
}
