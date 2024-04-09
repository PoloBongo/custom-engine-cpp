#include "Modules/RHIVulkanModule.h"

#include "Transform.h"
#include "ModuleManager.h"
#include "Modules/WindowModule.h"

RHIVulkanModule::RHIVulkanModule()
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}
void RHIVulkanModule::Init()
{

}

void RHIVulkanModule::Start()
{
	p_lveWindow = moduleManager->GetModule<WindowModule>()->GetWindow();
	p_lveDevice = moduleManager->GetModule<WindowModule>()->GetDevice();
	p_lveRenderer = moduleManager->GetModule<WindowModule>()->GetRenderer();

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

	viewerObject = new GameObject(GameObject::CreateGameObject());
	viewerObject->GetTransform()->SetPosition({ 0.f,0.f,-2.5f });
}

void RHIVulkanModule::FixedUpdate()
{
}

void RHIVulkanModule::Update()
{
}

void RHIVulkanModule::PreRender()
{
	currentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
	p_lveRenderer->BeginSwapChainRenderPass(*currentCommandBuffer);
}

void RHIVulkanModule::Render()
{
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
