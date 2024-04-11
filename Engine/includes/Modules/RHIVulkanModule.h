#pragma once
// vulkan_rhi.h
#pragma once

#include <vulkan/vulkan.hpp>

#include "keyboard_movement_controller.h"
#include "rhi.h"

#include "lve_descriptors.h"
#include "lve_model.h"
#include "lve_pipeline.h"
#include "lve_renderer.h"
#include "Systems/simple_render_system.h"
#include "Systems/point_light_system.h"
#include "Modules/WindowModule.h"


class RHIVulkanModule final : public RHIModule
{
	public:

		RHIVulkanModule();
		~RHIVulkanModule() override;

		vk::CommandBuffer* GetCurrentCommandBuffer() const { return currentCommandBuffer.get(); }

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
		// Autres méthodes pour la création de la surface, des périphériques logiques, etc.

		std::unique_ptr<vk::CommandBuffer> currentCommandBuffer;

		lve::SimpleRenderSystem* simpleRenderSystem;
		lve::PointLightSystem* pointLightSystem;

		lve::LveDescriptorPool::Builder* builder;

		std::unique_ptr<lve::LveDescriptorSetLayout, std::default_delete<lve::LveDescriptorSetLayout>>* globalSetLayout;

		lve::LveCamera* camera;
		lve::KeyboardMovementController       cameraController{};
		GameObject* viewerObject;

		std::vector<vk::DescriptorSet>               globalDescriptorSets;
		std::vector<std::unique_ptr<lve::LveBuffer>> uboBuffers;
		std::vector<GameObject*>                gameObjects;

		// note : order of declarations matters
		std::unique_ptr<lve::LveDescriptorPool> globalPool{};
		int frameIndex;
		lve::GlobalUbo ubo{};

		WindowModule*                windowModule = nullptr;
		lve::LveSwapChain*           p_lveSwapChain;
		lve::LveModel*               p_lveModel;
		lve::LvePipeline*            p_lvePipeline;
		lve::LveBuffer*              p_lveBuffer;
		lve::LveDescriptorPool*      p_lveDescriptorPool;
		lve::LveDescriptorSetLayout* p_lveDescriptorSetLayout;
		lve::LveDescriptorWriter*    p_lveDescriptorWriter;
		// Autres membres spécifiques à Vulkan
};
