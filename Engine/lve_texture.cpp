#include "lve_texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>

bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good(); // Check if the file stream is in a good state (i.e., file exists)
}

namespace lve {
	LveTexture::LveTexture(LveDevice& _device, const std::string& filepath) : lveDevice(_device) {
		int channels;
		int bytesPerPixel;

		std::string filePath = filepath; // Replace this with your file path
		if (fileExists(filePath)) {
			std::cout << "File exists at path: " << filePath << std::endl;
		}
		else {
			std::cout << "File does not exist at path: " << filePath << std::endl;
		}

		// symbole externe non résolu stbi_load
		stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, 4);

		if (data == nullptr) {
			// Gérer l'erreur de chargement de l'image
		}

		for (int i = 0; i < width * height * 4; i += 4) {
			unsigned char temp = data[i];
			data[i] = data[i + 2];
			data[i + 3] = temp;
		}

		mipLevels = std::floor(std::log2(std::max(width, height))) + 1;

		LveBuffer stagingBuffer(lveDevice, 4,
			static_cast<uint32_t>(width * height),
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(data);

		imageFormat = vk::Format::eR8G8B8A8Srgb;//eR8G8B8A8Srgb // eB8G8R8A8Srgb

		vk::ImageCreateInfo imageInfo{};
		imageInfo.sType = vk::StructureType::eImageCreateInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.format = imageFormat;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.setExtent({ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 });
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

		lveDevice.CreateImageWithInfo(imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, image, imageMemory);

		transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		lveDevice.CopyBufferToImage(stagingBuffer.GetBuffer(), image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1);

		//transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		generateMipmaps();

		imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::SamplerCreateInfo samplerInfo{};
		samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.compareOp = vk::CompareOp::eNever;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.maxAnisotropy = 4.0;
		samplerInfo.anisotropyEnable = vk::True;
		samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;

		lveDevice.Device().createSampler(&samplerInfo, nullptr, &sampler);

		vk::ImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = vk::StructureType::eImageViewCreateInfo;
		imageViewInfo.viewType = vk::ImageViewType::e2D;
		imageViewInfo.format = imageFormat;
		imageViewInfo.components = (vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA );
		imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;
		imageViewInfo.subresourceRange.levelCount = mipLevels;
		imageViewInfo.image = image;

		lveDevice.Device().createImageView(&imageViewInfo, nullptr, &imageView);
		//vk::Result result = lveDevice.device().createImageView(&imageViewInfo, nullptr, &imageView);
		//if (result != vk::Result::eSuccess) {
		//	throw std::runtime_error("ERROR creating image view");
		//}

		stbi_image_free(data); // symbole externe non résolu stbi_image_free

	}

	LveTexture::~LveTexture() {
		std::cout << "Destroyed !!";
		
		lveDevice.Device().destroyImage(image, nullptr);
		lveDevice.Device().freeMemory(imageMemory, nullptr);
		lveDevice.Device().destroyImageView(imageView, nullptr);
		lveDevice.Device().destroySampler(sampler, nullptr);
	}

	void LveTexture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::CommandBuffer commandBuffer = lveDevice.BeginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier{};
		barrier.sType = vk::StructureType::eImageMemoryBarrier;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}

		else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else {
			throw std::runtime_error("unsuppported layout transition!");
		}

		//vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		//commandBuffer.CmdpipelineBarrier(sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		commandBuffer.pipelineBarrier(
			sourceStage,                      // Source pipeline stage
			destinationStage,                 // Destination pipeline stage
			vk::DependencyFlags{},            // Dependency flags
			nullptr,                          // Memory barriers
			nullptr,                          // Buffer memory barriers
			barrier                          // Image memory barriers
		);

		lveDevice.EndSingleTimeCommands(commandBuffer);
	}

	void LveTexture::generateMipmaps() {
		vk::FormatProperties formatProperties;
		lveDevice.GetPhysicalDevice().getFormatProperties(imageFormat, &formatProperties);
		
		if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		vk::CommandBuffer commandBuffer = lveDevice.BeginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier{};
		barrier.sType = vk::StructureType::eImageMemoryBarrier;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
				vk::PipelineStageFlagBits::eTransfer,                      // dstStageMask
				vk::DependencyFlags{},                                     // dependencyFlags
				nullptr,                                                   // memoryBarriers
				nullptr,                                                   // bufferMemoryBarriers
				barrier,                                                   // imageMemoryBarriers
				{}                                                         // dispatch
			);

			vk::ImageBlit blit{};
			blit.srcOffsets[0].x = 0;
			blit.srcOffsets[0].y = 0;
			blit.srcOffsets[0].z = 0;
			blit.srcOffsets[1].x = mipWidth;
			blit.srcOffsets[1].y = mipHeight;
			blit.srcOffsets[1].z = 1;
			blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0].x = 0;
			blit.dstOffsets[0].y = 0;
			blit.dstOffsets[0].z = 0;
			blit.dstOffsets[1].x = mipWidth > 1 ? mipWidth / 2 : 1;
			blit.dstOffsets[1].y = mipHeight > 1 ? mipHeight / 2 : 1;
			blit.dstOffsets[1].z = 1;
			blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
				vk::PipelineStageFlagBits::eFragmentShader,                      // dstStageMask
				vk::DependencyFlags{},                                     // dependencyFlags
				nullptr,                                                   // memoryBarriers
				nullptr,                                                   // bufferMemoryBarriers
				barrier,                                                   // imageMemoryBarriers
				{}                                                         // dispatch
			);
			
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;

		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
			vk::PipelineStageFlagBits::eFragmentShader,                      // dstStageMask
			vk::DependencyFlags{},                                     // dependencyFlags
			nullptr,                                                   // memoryBarriers
			nullptr,                                                   // bufferMemoryBarriers
			barrier,                                                   // imageMemoryBarriers
			{}                                                         // dispatch
		);

		lveDevice.EndSingleTimeCommands(commandBuffer);

	}

	//void LveTexture::createTextureDescriptorSet(vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout textureDescriptorSetLayout) {

	//	vk::DescriptorSetAllocateInfo allocInfo{};
	//	allocInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
	//	allocInfo.descriptorPool = descriptorPool;
	//	allocInfo.descriptorSetCount = 1;
	//	allocInfo.pSetLayouts = &textureDescriptorSetLayout;
	//	
	//	if (lveDevice.Device().allocateDescriptorSets(&allocInfo, &descriptorSet) != vk::Result::eSuccess) {
	//		throw std::runtime_error("failed to allocate descriptor sets !");
	//	}

	//	vk::DescriptorImageInfo imageInfo{};
	//	imageInfo.sampler = getSampler();
	//	imageInfo.imageView = getImageView();
	//	imageInfo.imageLayout = getImageLayout();

	//	vk::WriteDescriptorSet descriptorWrite = {};
	//	descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
	//	descriptorWrite.dstSet = descriptorSet; 
	//	descriptorWrite.dstBinding = 1;
	//	descriptorWrite.dstArrayElement = 0;
	//	descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	//	descriptorWrite.descriptorCount = 1;
	//	descriptorWrite.pImageInfo = &imageInfo;

	//	lveDevice.Device().updateDescriptorSets(1,&descriptorWrite, 0, nullptr);

	//}

}