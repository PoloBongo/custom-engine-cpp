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
//VkImage HUDModule::createTextureImage(const std::string& texturePath) {
//    int texWidth, texHeight, texChannels;
//    stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//    if (!pixels) {
//        throw std::runtime_error("Failed to load texture image: " + texturePath);
//    }
//
//    VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
//    memcpy(data, pixels, static_cast<size_t>(imageSize));
//    vkUnmapMemory(device, stagingBufferMemory);
//
//    stbi_image_free(pixels);
//
//    VkImage textureImage;
//    VkDeviceMemory textureImageMemory;
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
VkImageView HUDModule::CreateTextureImageView(const VkImage _image)
{
	return CreateImageView(_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

// Méthode pour créer un échantillonneur Vulkan pour les textures
VkSampler HUDModule::CreateTextureSampler() const
{
	VkSamplerCreateInfo sampler_info{};
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

	VkSampler sampler;
	if (vkCreateSampler(device, &sampler_info, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create texture sampler");

	return sampler;
}

HUDModule::HUDModule(const VkDevice _device, const VkRenderPass _renderPass, const VkQueue _graphicsQueue, const VkCommandPool _commandPool): device(
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
void HUDModule::Render(VkCommandBuffer _commandBuffer)
{
	// Code pour dessiner les composants HUD avec Vulkan
	// Utiliser les commandes Vulkan pour dessiner les textures des composants sur l'écran
}

void HUDModule::TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout)
{
}

void HUDModule::CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties,
                             VkBuffer&    _buffer, VkDeviceMemory&  _bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = _size;
	bufferInfo.usage       = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &_bufferMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate buffer memory");

	vkBindBufferMemory(device, _buffer, _bufferMemory, 0);
}

void HUDModule::CreateImage(VkDeviceSize          _size, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage,
                            VkMemoryPropertyFlags _properties, VkImage& _image, VkDeviceMemory& _imageMemory)
{
	VkImageCreateInfo imageInfo{};
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

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, _image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &_imageMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate image memory");

	vkBindImageMemory(device, _image, _imageMemory, 0);
}

void HUDModule::CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height)
{
	VkCommandBuffer commandBuffer;

	// Allouer un tampon de commande
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool        = commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	// Commencer l'enregistrement des commandes
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Copier le tampon vers l'image
	VkBufferImageCopy region{};
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
	VkSubmitInfo submitInfo{};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	// Libérer le tampon de commande
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

VkImageView HUDModule::CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags)
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
