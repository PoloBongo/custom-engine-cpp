#pragma once

#include "lve_device.h"
#include "lve_buffer.h"

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

namespace lve {
	class LveImage
	{
	public:

		/**
		 * @brief Constructeur de la classe LveImage.
		 *
		 * @param _lveDevice Référence vers l'objet LveDevice associé à cette image.
		 */
		LveImage(LveDevice& _lveDevice);

		/** @brief Destructeur de la classe LveImage. */
		~LveImage();

		/**
		 * @brief Opérateur de copie supprimé pour la classe LveImage.
		 *
		 * @return L'objet LveImage résultant de l'opération.
		 */
		LveImage(const LveImage&) = delete;

		/**
		 * @brief Opérateur d'affectation supprimé pour la classe LveImage.
		 *
		 * @return L'objet LveImage résultant de l'opération.
		 */
		LveImage operator=(const LveImage&) = delete;

	private:
		/**
		 * @brief Crée une image de texture.
		 */
		void createTextureImage();

		/**
		 * @brief Crée une image Vulkan avec les paramètres spécifiés.
		 *
		 * @param width La largeur de l'image.
		 * @param height La hauteur de l'image.
		 * @param format Le format de l'image.
		 * @param tiling Le mode de pavage de l'image.
		 * @param usage Les indicateurs d'utilisation de l'image.
		 * @param properties Les propriétés de mémoire de l'image.
		 * @param image Référence où stocker l'image Vulkan créée.
		 * @param imageMemory Référence où stocker la mémoire allouée pour l'image.
		 */
		void createImage(uint32_t width,
			uint32_t height,
			vk::Format format,
			vk::ImageTiling tiling,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::Image& image,
			vk::DeviceMemory& imageMemory);

		/** Référence vers l'objet LveDevice associé. */
		LveDevice& lveDevice;

		/** Image de texture Vulkan créée. */
		vk::Image textureImage;

		/** Mémoire allouée pour l'image de texture. */
		vk::DeviceMemory textureImageMemory;

	};
}
