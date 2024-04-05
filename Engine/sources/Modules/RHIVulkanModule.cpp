#include "Modules/RHIVulkanModule.h"

RHIVulkanModule::RHIVulkanModule() : Instance(VK_NULL_HANDLE), m_DebugMessenger(VK_NULL_HANDLE)
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}

bool RHIVulkanModule::CreateVulkanInstance()
{
	Instance = new vk::Instance(p_lveDevice->CreateInstance());
	if (Instance != VK_NULL_HANDLE)
		// Gestion des erreurs
		return false;

	return true;
}

bool RHIVulkanModule::SetupDebugMessenger()
{
	m_DebugMessenger = new vk::DebugUtilsMessengerEXT(p_lveDevice->SetupDebugMessenger());
	if (m_DebugMessenger != VK_NULL_HANDLE)
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
	m_CurrentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
	p_lveRenderer->BeginSwapChainRenderPass(*m_CurrentCommandBuffer);
}

void RHIVulkanModule::Render()
{
}

void RHIVulkanModule::RenderGui()
{
}

void RHIVulkanModule::PostRender()
{
	p_lveRenderer->EndSwapChainRenderPass(*m_CurrentCommandBuffer);
	p_lveRenderer->EndFrame(m_CurrentCommandBuffer.get());
}

void RHIVulkanModule::Release()
{
	if (Instance != VK_NULL_HANDLE)
	{
		Instance->destroy();
		delete Instance;
	}
	if (Instance != VK_NULL_HANDLE)
	{
		Instance->destroy();
		delete Instance;
	}
}

void RHIVulkanModule::Finalize()
{
}
