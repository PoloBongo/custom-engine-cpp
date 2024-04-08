#include "Modules/RHIVulkanModule.h"

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
