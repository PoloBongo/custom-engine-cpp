#include "Modules/HUDModule.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "stb_image.h"


// Fonction utilitaire pour charger une image depuis un fichier sur le disque
std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filename);

	size_t            fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

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
//    vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &data);
//    memcpy(data, pixels, static_cast<size_t>(imageSize));
//    vkUnmapMemory(m_device, stagingBufferMemory);
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
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//    vkFreeMemory(m_device, stagingBufferMemory, nullptr);
//
//    return textureImage;
//}

// Méthode pour créer une vue d'image Vulkan à partir d'une image Vulkan
VkImageView HUDModule::createTextureImageView(VkImage image)
{
	return createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

// Méthode pour créer un échantillonneur Vulkan pour les textures
VkSampler HUDModule::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter               = VK_FILTER_LINEAR;
	samplerInfo.minFilter               = VK_FILTER_LINEAR;
	samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable        = VK_TRUE;
	samplerInfo.maxAnisotropy           = 16;
	samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable           = VK_FALSE;
	samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias              = 0.0f;
	samplerInfo.minLod                  = 0.0f;
	samplerInfo.maxLod                  = 0.0f;

	VkSampler sampler;
	if (vkCreateSampler(m_device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create texture sampler");

	return sampler;
}

// Méthode pour ajouter un composant HUD
void HUDModule::addComponent(float x, float y, float width, float height, const std::string& texturePath)
{
	HUDComponent component{};
	component.x      = x;
	component.y      = y;
	component.width  = width;
	component.height = height;
	//component.texture = createTextureImage(texturePath);
	component.imageView = createTextureImageView(component.texture);
	component.sampler   = createTextureSampler();

	m_components.push_back(component);
}

// Méthode pour nettoyer les composants HUD
void HUDModule::cleanupComponents()
{
	for (auto& component : m_components)
	{
		vkDestroySampler(m_device, component.sampler, nullptr);
		vkDestroyImageView(m_device, component.imageView, nullptr);
		vkDestroyImage(m_device, component.texture, nullptr);
	}
	m_components.clear();
}

// Méthode pour dessiner les composants HUD
void HUDModule::render(VkCommandBuffer commandBuffer)
{
	// Code pour dessiner les composants HUD avec Vulkan
	// Utiliser les commandes Vulkan pour dessiner les textures des composants sur l'écran
}

void HUDModule::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
}

void HUDModule::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                             VkBuffer&    buffer, VkDeviceMemory&  bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate buffer memory");

	vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void HUDModule::createImage(VkDeviceSize          size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                            VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType     = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width  = WIDTH;
	imageInfo.extent.height = HEIGHT;
	imageInfo.extent.depth  = 1;
	imageInfo.mipLevels     = 1;
	imageInfo.arrayLayers   = 1;
	imageInfo.format        = format;
	imageInfo.tiling        = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage         = usage;
	imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error(
			"Failed to allocate image memory");

	vkBindImageMemory(m_device, image, imageMemory, 0);
}

void HUDModule::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer;

	// Allouer un tampon de commande
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool        = m_commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

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
	region.imageExtent                     = {width, height, 1};
	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	// Terminer l'enregistrement des commandes
	vkEndCommandBuffer(commandBuffer);

	// Soumettre le tampon de commande pour exécution
	VkSubmitInfo submitInfo{};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;
	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	// Libérer le tampon de commande
	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

VkImageView HUDModule::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
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
