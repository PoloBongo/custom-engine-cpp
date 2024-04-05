#include "Modules/HUDModule.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "stb_image.h"


// Fonction utilitaire pour charger une image depuis un fichier sur le disque
std::vector<char> HUDModule::ReadFile(const std::string& _filename)
{
	std::ifstream file(_filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) throw std::runtime_error("Failed to open file: " + _filename);

	const size_t            file_size = file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

// Méthode pour créer une image Vulkan à partir d'un fichier image sur le disque
//vk::Image HUDModule::createTextureImage(const std::string& texturePath) {
//    int texWidth, texHeight, texChannels;
//    stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//    if (!pixels) {
//        throw std::runtime_error("Failed to load texture image: " + texturePath);
//    }
//
//    vk::DeviceSize imageSize = texWidth * texHeight * 4;
//
//    vk::Buffer stagingBuffer;
//    vk::DeviceMemory stagingBufferMemory;
//    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
//    memcpy(data, pixels, static_cast<size_t>(imageSize));
//    vkUnmapMemory(device, stagingBufferMemory);
//
//    stbi_image_free(pixels);
//
//    vk::Image textureImage;
//    vk::DeviceMemory textureImageMemory;
//    createImage(imageSize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
//
//    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//    vkDestroyBuffer(device, stagingBuffer, nullptr);
//    vkFreeMemory(device, stagingBufferMemory, nullptr);
//
//    return textureImage;
//}

// Méthode pour créer une vue d'image Vulkan à partir d'une image Vulkan
vk::ImageView HUDModule::CreateTextureImageView(const vk::Image _image)
{
	return CreateImageView(_image, vk::Format::eR8G8B8A8Unorm , vk::ImageAspectFlagBits::eColor);
}

// Méthode pour créer un échantillonneur Vulkan pour les textures
vk::Sampler HUDModule::CreateTextureSampler() const
{
	vk::SamplerCreateInfo sampler_info{};
	sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter               = VK_FILTER_LINEAR;
	sampler_info.minFilter               = VK_FILTER_LINEAR;
	sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable        = VK_TRUE;
	sampler_info.maxAnisotropy           = 16;
	sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable           = VK_FALSE;
	sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias              = 0.0f;
	sampler_info.minLod                  = 0.0f;
	sampler_info.maxLod                  = 0.0f;

	vk::Sampler sampler;
	if (vkCreateSampler(device, &sampler_info, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create texture sampler");

	return sampler;
}

HUDModule::HUDModule(const vk::Device _device, const vk::RenderPass _renderPass, const vk::Queue _graphicsQueue, const vk::CommandPool _commandPool): device(
	                                                                                                                                             _device), renderPass(
	                                                                                                                                             _renderPass), graphicsQueue(
	                                                                                                                                             _graphicsQueue), commandPool(
	                                                                                                                                             _commandPool)
{
}

// Méthode pour ajouter un composant HUD
void HUDModule::AddComponent(float _x, float _y, float _width, float _height, const std::string& _texturePath)
{
	HUDComponent component{};
	component.x      = _x;
	component.y      = _y;
	component.width  = _width;
	component.height = _height;
	//component.texture = createTextureImage(texturePath);
	component.imageView = CreateTextureImageView(component.texture);
	component.sampler   = CreateTextureSampler();

	components.push_back(component);
}

void HUDModule::RemoveComponent()
{

}

// Méthode pour nettoyer les composants HUD
void HUDModule::CleanupComponents()
{
	for (auto& component : components)
	{
		vkDestroySampler(device, component.sampler, nullptr);
		vkDestroyImageView(device, component.imageView, nullptr);
		vkDestroyImage(device, component.texture, nullptr);
	}
	components.clear();
}

// Méthode pour dessiner les composants HUD
void HUDModule::Render(vk::CommandBuffer _commandBuffer)
{
	// Code pour dessiner les composants HUD avec Vulkan
	// Utiliser les commandes Vulkan pour dessiner les textures des composants sur l'écran
}

void HUDModule::TransitionImageLayout(vk::Image _image, vk::Format _format, vk::ImageLayout _oldLayout, vk::ImageLayout _newLayout)
{
}

void HUDModule::CreateBuffer(vk::DeviceSize _size, vk::BufferUsageFlags _usage, vk::MemoryPropertyFlags _properties,
                             vk::Buffer&    _buffer, vk::DeviceMemory&  _bufferMemory)
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = _size;
	bufferInfo.usage       = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create buffer");

	vk::MemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &_bufferMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate buffer memory");

	vkBindBufferMemory(device, _buffer, _bufferMemory, 0);
}

void HUDModule::CreateImage(vk::DeviceSize          _size, vk::Format _format, vk::ImageTiling _tiling, vk::ImageUsageFlags _usage,
                            vk::MemoryPropertyFlags _properties, vk::Image& _image, vk::DeviceMemory& _imageMemory)
{
	vk::ImageCreateInfo imageInfo{};
	imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType     = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width  = WIDTH;
	imageInfo.extent.height = HEIGHT;
	imageInfo.extent.depth  = 1;
	imageInfo.mipLevels     = 1;
	imageInfo.arrayLayers   = 1;
	imageInfo.format        = _format;
	imageInfo.tiling        = _tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage         = _usage;
	imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &_image) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create image");

	vk::MemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, _image, &memRequirements);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &_imageMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate image memory");

	vkBindImageMemory(device, _image, _imageMemory, 0);
}

void HUDModule::CopyBufferToImage(vk::Buffer _buffer, vk::Image _image, uint32_t _width, uint32_t _height)
{
	vk::CommandBuffer commandBuffer;

	// Allouer un tampon de commande
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool        = commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	// Commencer l'enregistrement des commandes
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Copier le tampon vers l'image
	vk::BufferImageCopy region{};
	region.bufferOffset                    = 0;
	region.bufferRowLength                 = 0;
	region.bufferImageHeight               = 0;
	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;
	region.imageOffset                     = {0, 0, 0};
	region.imageExtent                     = {_width, _height, 1};
	vkCmdCopyBufferToImage(commandBuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	// Terminer l'enregistrement des commandes
	vkEndCommandBuffer(commandBuffer);

	// Soumettre le tampon de commande pour exécution
	vk::SubmitInfo submitInfo{};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	// Libérer le tampon de commande
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

vk::ImageView HUDModule::CreateImageView(vk::Image _image, vk::Format _format, vk::ImageAspectFlags _aspectFlags)
{
	// Implémentation de la création de la vue d'image Vulkan
	return nullptr;
}


void HUDModule::Init()
{
	Module::Init();
}

void HUDModule::Start()
{
	Module::Start();
}

void HUDModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void HUDModule::Update()
{
	Module::Update();
}

void HUDModule::PreRender()
{
	Module::PreRender();
}

void HUDModule::Render()
{
	Module::Render();
}

void HUDModule::RenderGui()
{
	Module::RenderGui();
}

void HUDModule::PostRender()
{
	Module::PostRender();
}

void HUDModule::Release()
{
	Module::Release();
}

void HUDModule::Finalize()
{
	Module::Finalize();
}
