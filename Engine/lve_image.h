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

		struct Builder {
		};

		LveImage(LveDevice& _lveDevice);
		~LveImage();

		LveImage(const LveImage&) = delete;
		LveImage operator=(const LveImage&) = delete;

	private:
		void createTextureImage();

		void createImage(uint32_t width, 
			uint32_t height, 
			vk::Format format, 
			vk::ImageTiling tiling, 
			vk::ImageUsageFlags usage, 
			vk::MemoryPropertyFlags properties, 
			vk::Image& image,
			vk::DeviceMemory& imageMemory);

		LveDevice& lveDevice;

		vk::Image textureImage;
		vk::DeviceMemory textureImageMemory;
	};
}
