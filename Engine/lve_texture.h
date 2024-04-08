#pragma once
#include "lve_device.h"
#include "lve_buffer.h"
#include <string>

#include <stdexcept>

namespace lve {
	class LveTexture
	{
	public: 
		LveTexture(LveDevice& _device, const std::string& filepath);
		~LveTexture();

		LveTexture(const LveTexture&) = delete;
		LveTexture& operator=(const LveTexture&) = delete;
		LveTexture(LveTexture&&) = delete;
		LveTexture& operator=(LveTexture&&) = delete;

		vk::Sampler getSampler() { return sampler; }
		vk::ImageView getImageView() { return imageView; }
		vk::ImageLayout getImageLayout() { return imageLayout; }

	private:

		void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		void generateMipmaps();

		int width, height, mipLevels;
		LveDevice& lveDevice;
		vk::Image image;
		vk::DeviceMemory imageMemory;
		vk::ImageView imageView;
		vk::Sampler sampler;
		vk::Format imageFormat;
		vk::ImageLayout imageLayout;
	};
}

