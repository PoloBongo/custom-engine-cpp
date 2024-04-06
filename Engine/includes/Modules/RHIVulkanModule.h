#pragma once
// vulkan_rhi.h
#pragma once

#include <vulkan/vulkan.hpp>
#include "rhi.h"

#include "lve_descriptors.h"
#include "lve_device.h"
#include "lve_model.h"
#include "lve_pipeline.h"
#include "lve_renderer.h"

class RHIVulkanModule final : public RHIModule
{
	public:
		static constexpr int WIDTH  = 800; ///< Largeur de la fenêtre par défaut.
		static constexpr int HEIGHT = 600; ///< Hauteur de la fenêtre par défaut.

		RHIVulkanModule();
		~RHIVulkanModule() override;

		/**
		* @brief Initialise le module.
		*/
		void Init() override;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

	private:
		// Méthodes et membres privés spécifiques à Vulkan
		bool CreateVulkanInstance();
		bool SetupDebugMessenger();
		// Autres méthodes pour la création de la surface, des périphériques logiques, etc.

		vk::Instance*                      instance;
		vk::DebugUtilsMessengerEXT*        debugMessenger;
		std::unique_ptr<vk::CommandBuffer> currentCommandBuffer;


		lve::LveDevice*              p_lveDevice;
		lve::LveWindow*              p_lveWindow;
		lve::LveRenderer*            p_lveRenderer;
		lve::LveSwapChain*           p_lveSwapChain;
		lve::LveModel*               p_lveModel;
		lve::LvePipeline*            p_lvePipeline;
		lve::LveBuffer*              p_lveBuffer;
		lve::LveDescriptorPool*      p_lveDescriptorPool;
		lve::LveDescriptorSetLayout* p_lveDescriptorSetLayout;
		lve::LveDescriptorWriter*    p_lveDescriptorWriter;
		// Autres membres spécifiques à Vulkan
};
