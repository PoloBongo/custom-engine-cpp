#pragma once

#include "lve_buffer.h"
#include "lve_device.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace lve
{
	class LveImage
	{
		public:
			/**
			 * @brief Constructeur de la classe LveImage.
			 *
			 * @param _lveDevice Référence vers l'objet LveDevice associé à cette image.
			 */
			explicit LveImage(LveDevice& _lveDevice);

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
			void CreateTextureImage();

			/**
			 * @brief Crée une image Vulkan avec les paramètres spécifiés.
			 *
			 * @param _width La largeur de l'image.
			 * @param _height La hauteur de l'image.
			 * @param _format Le format de l'image.
			 * @param _tiling Le mode de pavage de l'image.
			 * @param _usage Les indicateurs d'utilisation de l'image.
			 * @param _properties Les propriétés de mémoire de l'image.
			 * @param _image Référence où stocker l'image Vulkan créée.
			 * @param _imageMemory Référence où stocker la mémoire allouée pour l'image.
			 */
			void CreateImage(uint32_t                _width,
			                 uint32_t                _height,
			                 vk::Format              _format,
			                 vk::ImageTiling         _tiling,
			                 vk::ImageUsageFlags     _usage,
			                 vk::MemoryPropertyFlags _properties,
			                 vk::Image&              _image,
			                 vk::DeviceMemory&       _imageMemory);

			/** Référence vers l'objet LveDevice associé. */
			LveDevice& lveDevice;

			/** Image de texture Vulkan créée. */
			vk::Image textureImage;

			/** Mémoire allouée pour l'image de texture. */
			vk::DeviceMemory textureImageMemory;
	};
}
