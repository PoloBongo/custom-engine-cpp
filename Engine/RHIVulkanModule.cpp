#include "RHIVulkanModule.h"

RHIVulkanModule::RHIVulkanModule() : m_Instance(VK_NULL_HANDLE), m_DebugMessenger(VK_NULL_HANDLE)
{
}

RHIVulkanModule::~RHIVulkanModule() {
    Release();
}

bool RHIVulkanModule::CreateVulkanInstance() {
 
    m_Instance = new vk::Instance(p_lveDevice->CreateInstance());
    if (m_Instance != VK_NULL_HANDLE) {
        // Gestion des erreurs
        return false;
    }

    return true;
}

bool RHIVulkanModule::SetupDebugMessenger() {
    m_DebugMessenger = new vk::DebugUtilsMessengerEXT(p_lveDevice->SetupDebugMessenger());
    if (m_DebugMessenger != VK_NULL_HANDLE) {
        // Gestion des erreurs
        return false;
    }

    return true;
}

/**
* @brief Initialise le module.
*
 */
void RHIVulkanModule::Init()
{
    p_lveWindow = new lve::LveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
    p_lveDevice = new lve::LveDevice{ *p_lveWindow };

    if (!CreateVulkanInstance()) {
        throw std::runtime_error("Failed");
    }

    if (!SetupDebugMessenger()) {
        throw std::runtime_error("Failed");
    }

    // Autres étapes d'initialisation de Vulkan
}

/**
 * @brief Démarre le module.
 */
void RHIVulkanModule::Start(){}

/**
 * @brief Effectue une mise à jour fixe du module.
 */
void RHIVulkanModule::FixedUpdate(){}

/**
 * @brief Met à jour le module.
 */
void RHIVulkanModule::Update(){}

/**
 * @brief Fonction pré-rendu du module.
 */
void RHIVulkanModule::PreRender()
{
    m_CurrentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
    p_lveRenderer->BeginSwapChainRenderPass(*m_CurrentCommandBuffer);
}

/**
 * @brief Rendu du module.
 */
void RHIVulkanModule::Render(){}

/**
 * @brief Rendu de l'interface graphique du module.
 */
void RHIVulkanModule::RenderGui(){}

/**
 * @brief Fonction post-rendu du module.
 */
void RHIVulkanModule::PostRender()
{
    p_lveRenderer->EndSwapChainRenderPass(*m_CurrentCommandBuffer);
    p_lveRenderer->EndFrame(m_CurrentCommandBuffer.get());
}

/**
 * @brief Libère les ressources utilisées par le module.
 */
void RHIVulkanModule::Release()
{

    if (m_Instance != VK_NULL_HANDLE) {
        m_Instance->destroy();
        delete m_Instance;
    }
    if (m_Instance != VK_NULL_HANDLE) {
        m_Instance->destroy();
        delete m_Instance;
    }
}

/**
 * @brief Finalise le module.
 */
void RHIVulkanModule::Finalize()
{
	
}
