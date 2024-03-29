#include "Managers/HUDManager.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include "stb_image.h"


// Fonction utilitaire pour charger une image depuis un fichier sur le disque
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

// Méthode pour créer une image Vulkan à partir d'un fichier image sur le disque
//VkImage HUDManager::createTextureImage(const std::string& texturePath) {
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
VkImageView HUDManager::createTextureImageView(VkImage image) {
    return createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

// Méthode pour créer un échantillonneur Vulkan pour les textures
VkSampler HUDManager::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler sampler;
    if (vkCreateSampler(m_device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler");
    }

    return sampler;
}

// Méthode pour ajouter un composant HUD
void HUDManager::addComponent(float x, float y, float width, float height, const std::string& texturePath) {
    HUDComponent component{};
    component.x = x;
    component.y = y;
    component.width = width;
    component.height = height;
    //component.texture = createTextureImage(texturePath);
    component.imageView = createTextureImageView(component.texture);
    component.sampler = createTextureSampler();

    m_components.push_back(component);
}

// Méthode pour nettoyer les composants HUD
void HUDManager::cleanupComponents() {
    for (auto& component : m_components) {
        vkDestroySampler(m_device, component.sampler, nullptr);
        vkDestroyImageView(m_device, component.imageView, nullptr);
        vkDestroyImage(m_device, component.texture, nullptr);
    }
    m_components.clear();
}

// Méthode pour dessiner les composants HUD
void HUDManager::render(VkCommandBuffer commandBuffer) {
    // Code pour dessiner les composants HUD avec Vulkan
    // Utiliser les commandes Vulkan pour dessiner les textures des composants sur l'écran
}

void HUDManager::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

}

void HUDManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    // Implémentation de la création du tampon Vulkan
}
void HUDManager::createImage(VkDeviceSize size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    // Implémentation de la création de l'image Vulkan
}

void HUDManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    // Implémentation de la copie du tampon vers l'image Vulkan
}

VkImageView HUDManager::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    // Implémentation de la création de la vue d'image Vulkan
    return NULL;
}