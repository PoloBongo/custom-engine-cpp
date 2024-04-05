#include "Modules/RHIVulkanModule.h"

RHIVulkanModule::RHIVulkanModule() : instance(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE)
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}

bool RHIVulkanModule::CreateVulkanInstance()
{
	instance = new vk::Instance(p_lveDevice->CreateInstance());
	if (instance != VK_NULL_HANDLE)
		// Gestion des erreurs
		return false;

	return true;
}

bool RHIVulkanModule::SetupDebugMessenger()
{
	debugMessenger = new vk::DebugUtilsMessengerEXT(p_lveDevice->SetupDebugMessenger());
	if (debugMessenger != VK_NULL_HANDLE)
		// Gestion des erreurs
		return false;

	return true;
}

void RHIVulkanModule::Init()
{
	p_lveWindow = new lve::LveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
	p_lveDevice = new lve::LveDevice{*p_lveWindow};

	if (!CreateVulkanInstance()) throw std::runtime_error("Failed");

	if (!SetupDebugMessenger()) throw std::runtime_error("Failed");

	// Autres étapes d'initialisation de Vulkan
}

void RHIVulkanModule::Start()
{
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
	if (instance != VK_NULL_HANDLE)
	{
		instance->destroy();
		delete instance;
	}
	if (instance != VK_NULL_HANDLE)
	{
		instance->destroy();
		delete instance;
	}
}

void RHIVulkanModule::Finalize()
{
}
