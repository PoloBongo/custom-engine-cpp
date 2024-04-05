#pragma once
#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Module.h"

struct HUDComponent
{
	float       x,     y;      // Position de l'élément HUD
	float       width, height; // Largeur et hauteur de l'élément HUD
	VkImage     texture;       // Image Vulkan de l'élément HUD
	VkImageView imageView;     // Vue de l'image Vulkan
	VkSampler   sampler;       // Échantillonneur Vulkan
};

class HUDModule : public Module
{
	public:
		HUDModule(VkDevice device, VkRenderPass renderPass, VkQueue graphicsQueue, VkCommandPool commandPool);
		~HUDModule();

		void addComponent(float x, float y, float width, float height, const std::string& texturePath);
		void removeComponent(int index);
		void cleanupComponents();
		void render(VkCommandBuffer commandBuffer);

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

		static constexpr int HEIGHT = 600;
		static constexpr int WIDTH  = 800;

	private:
		VkDevice                  m_device;
		VkRenderPass              m_renderPass;
		VkQueue                   m_graphicsQueue;
		VkCommandPool             m_commandPool;
		std::vector<HUDComponent> m_components;

		// Méthodes pour créer et gérer les ressources Vulkan
		/*VkImage createTextureImage(const std::string& texturePath);*/
		VkImageView createTextureImageView(VkImage image);
		VkSampler createTextureSampler();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		                  VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void createImage(VkDeviceSize          size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		                 VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
};

#endif // HUD_MANAGER_H
