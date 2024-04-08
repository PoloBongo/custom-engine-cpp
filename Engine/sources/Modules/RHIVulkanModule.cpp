#include "Modules/RHIVulkanModule.h"

#include "ModuleManager.h"
#include "Modules/WindowModule.h"

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
	debugMessenger = new vk::DebugUtilsMessengerEXT(p_lveDevice->GetDebugMessenger());
	if (debugMessenger != VK_NULL_HANDLE)
		// Gestion des erreurs
		return false;

	return true;
}

void RHIVulkanModule::Init()
{

}

void RHIVulkanModule::Start()
{
	p_lveWindow = moduleManager->GetModule<WindowModule>()->GetWindow();
	p_lveDevice = moduleManager->GetModule<WindowModule>()->GetDevice();
	p_lveRenderer = moduleManager->GetModule<WindowModule>()->GetRenderer();
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
