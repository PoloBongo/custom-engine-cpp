#include "lve_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lve
{
	LveImage::LveImage(LveDevice& _lveDevice) : lveDevice(_lveDevice)
	{
		CreateTextureImage();
	}

	LveImage::~LveImage()
	{
	}

	void LveImage::CreateTextureImage()
	{
		int tex_width, tex_height, tex_channels;
		stbi_uc* pixels = stbi_load("textures/textures.jpg", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		const VkDeviceSize image_size = tex_width * tex_height * 4;

		if (!pixels) throw std::runtime_error("failed to load texture image");

		vk::Buffer       staging_buffer;
		vk::DeviceMemory staging_buffer_memory;

		lveDevice.CreateBuffer(image_size, vk::BufferUsageFlagBits::eTransferSrc,
		                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		                       staging_buffer, staging_buffer_memory);

		void* data;
		if (lveDevice.Device().mapMemory(staging_buffer_memory, 0, image_size, vk::MemoryMapFlags(), &data) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed");
		memcpy(data, pixels, image_size);
		lveDevice.Device().unmapMemory(staging_buffer_memory);
		stbi_image_free(pixels);

		CreateImage(tex_width, tex_height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
		            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
		            , vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

		lveDevice.CopyBufferToImage(staging_buffer, textureImage, static_cast<uint32_t>(tex_width),
		                            static_cast<uint32_t>(tex_height), 1);

		lveDevice.Device().destroyBuffer(staging_buffer);
		lveDevice.Device().freeMemory(staging_buffer_memory, nullptr);
	}

	void LveImage::CreateImage(const uint32_t                _width, const uint32_t _height, vk::Format _format,
	                           vk::ImageTiling               _tiling,
	                           vk::ImageUsageFlags           _usage,
	                           const vk::MemoryPropertyFlags _properties, vk::Image& _image,
	                           vk::DeviceMemory&             _imageMemory) const
	{
		vk::ImageCreateInfo image_info{};
		image_info.sType         = vk::StructureType::eImageCreateInfo;
		image_info.imageType     = vk::ImageType::e2D;
		image_info.extent.width  = _width;
		image_info.extent.height = _height;
		image_info.extent.depth  = 1;
		image_info.mipLevels     = 1;
		image_info.arrayLayers   = 1;
		image_info.format        = vk::Format::eR8G8B8A8Srgb;
		//VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
		//VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
		image_info.tiling        = vk::ImageTiling::eOptimal;
		image_info.initialLayout = vk::ImageLayout::eUndefined;
		image_info.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		image_info.sharingMode   = vk::SharingMode::eExclusive;
		image_info.samples       = vk::SampleCountFlagBits::e1;
		//imageInfo.flags = 0; // Optional

		if (lveDevice.Device().createImage(&image_info, nullptr, &_image) != vk::Result::eSuccess)
			throw
				std::runtime_error("failed to create image!");

		vk::MemoryRequirements mem_requirements;
		lveDevice.Device().getImageMemoryRequirements(_image, &mem_requirements);

		vk::MemoryAllocateInfo alloc_info{};
		alloc_info.sType           = vk::StructureType::eMemoryAllocateInfo;
		alloc_info.allocationSize  = mem_requirements.size;
		alloc_info.memoryTypeIndex = lveDevice.FindMemoryType(mem_requirements.memoryTypeBits, _properties);

		if (lveDevice.Device().allocateMemory(&alloc_info, nullptr, &_imageMemory) != vk::Result::eSuccess)
			throw
				std::runtime_error("failed to allocate image memory!");

		lveDevice.Device().bindImageMemory(_image, _imageMemory, 0);
	}
}
