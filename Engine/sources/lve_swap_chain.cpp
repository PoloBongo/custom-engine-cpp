#include "lve_swap_chain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

#include "VulkanHooks.h"

namespace lve
{
	LveSwapChain::LveSwapChain(LveDevice& deviceRef, vk::Extent2D extent) : lveDevice{deviceRef}, windowExtent{extent}
	{
		Init();
	}

	LveSwapChain::LveSwapChain(LveDevice&                    deviceRef, vk::Extent2D extent,
	                           std::shared_ptr<LveSwapChain> previous) : lveDevice{deviceRef}, windowExtent{extent},
	                                                                     oldSwapChain{previous}
	{
		Init();
		oldSwapChain = nullptr;
	}

	void LveSwapChain::Init()
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateFrameBuffers();
		CreateSyncObjects();
	}


	LveSwapChain::~LveSwapChain()
	{
		// Nettoie les image views de la chaîne de swap
		for (auto imageView : swapChainImageViews)
		{
			lveDevice.Device().destroyImageView(imageView, nullptr);
		}
		swapChainImageViews.clear();

		// Détruit la chaîne de swap
		if (swapChain != nullptr)
		{
			lveDevice.Device().destroySwapchainKHR(swapChain, nullptr);
			swapChain = nullptr;
		}

		// Nettoie les images de profondeur, leurs vues et la mémoire associée
		for (int i = 0; i < depthImages.size(); i++)
		{
			lveDevice.Device().destroyImageView(depthImageViews[i], nullptr);
			lveDevice.Device().destroyImage(depthImages[i], nullptr);
			lveDevice.Device().freeMemory(depthImageMemorys[i], nullptr);
		}

		// Nettoie les framebuffers de la chaîne de swap
		for (auto framebuffer : swapChainFramebuffers)
		{
			lveDevice.Device().destroyFramebuffer(framebuffer, nullptr);
		}

		// Détruit le render pass
		lveDevice.Device().destroyRenderPass(renderPass, nullptr);

		// Nettoie les objets de synchronisation
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			lveDevice.Device().destroySemaphore(renderFinishedSemaphores[i], nullptr);
			lveDevice.Device().destroySemaphore(imageAvailableSemaphores[i], nullptr);
			lveDevice.Device().destroyFence(inFlightFences[i], nullptr);
		}
	}

	vk::Result LveSwapChain::AcquireNextImage(uint32_t* imageIndex)
	{
		// Attend que les barrières de synchronisation soient satisfaites
		vk::Result result_ = lveDevice.Device().waitForFences(1, &inFlightFences[currentFrame], VK_TRUE,
		                                                      std::numeric_limits<uint64_t>::max());
		if (result_ != vk::Result::eSuccess) std::cout << "ALERTE" << std::endl;

		// Acquiert l'index de l'image suivante dans la chaîne de swap
		// en utilisant l'extension KHR_swapchain
		vk::Result result = lveDevice.Device().acquireNextImageKHR(
			swapChain,                              // La chaîne de swap
			std::numeric_limits<uint64_t>::max(),   // Durée d'attente infinie
			imageAvailableSemaphores[currentFrame], // Sémaphore non signalé (attendre qu'il soit signalé avant)
			nullptr,                                // Pas de gestion de l'objet de barrière de blocage
			imageIndex);                            // L'index de l'image acquise

		return result;
	}

	vk::Result LveSwapChain::SubmitCommandBuffers(const vk::CommandBuffer* buffers, uint32_t* imageIndex)
	{
		// Attend la fin des commandes en cours d'exécution pour l'image en cours d'acquisition
		if (imagesInFlight[*imageIndex] != nullptr)
		{
			vk::Result result = lveDevice.Device().waitForFences(1, &imagesInFlight[*imageIndex], VK_TRUE,
			                                                     std::numeric_limits<uint64_t>::max());
			if (result != vk::Result::eSuccess)
			{
				// Gérer l'erreur ou afficher un message d'erreur
			}
		}
		imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

		// Configuration de la soumission des commandes
		vk::SubmitInfo submitInfo;
		submitInfo.sType = vk::StructureType::eSubmitInfo;

		vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		// Déclaration d'un tableau de type VkPipelineStageFlags
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		// Initialisation du tableau avec un seul élément contenant le drapeau VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores    = waitSemaphores;
		submitInfo.pWaitDstStageMask  = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = buffers;

		vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount  = 1;
		submitInfo.pSignalSemaphores     = signalSemaphores;

		// Soumet les commandes à la file de commandes graphiques
		lveDevice.Device().resetFences(1, &inFlightFences[currentFrame]);
		if (lveDevice.GraphicsQueue().submit(1, &submitInfo, inFlightFences[currentFrame]) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to submit draw command buffer!");

		// Configuration de la présentation
		vk::PresentInfoKHR presentInfo;
		presentInfo.sType = vk::StructureType::ePresentInfoKHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = signalSemaphores;

		vk::SwapchainKHR swapChains[] = {swapChain};
		presentInfo.swapchainCount    = 1;
		presentInfo.pSwapchains       = swapChains;

		presentInfo.pImageIndices = imageIndex;

		// Présente l'image
		vk::Result result = lveDevice.PresentQueue().presentKHR(&presentInfo);

		// Passe au prochain frame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}


	void LveSwapChain::CreateSwapChain()
	{
		// Obtient les détails de prise en charge de la chaîne de swap
		SwapChainSupportDetails swapChainSupport = lveDevice.GetSwapChainSupport();

		// Choix du format de surface de la chaîne de swap
		vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);

		// Choix du mode de présentation de la chaîne de swap
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);

		// Choix de l'étendue de la chaîne de swap
		vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		// Détermine le nombre d'images dans la chaîne de swap
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
		    imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		// Configuration de la création de la chaîne de swap
		vk::SwapchainCreateInfoKHR createInfo = {};
		createInfo.sType                      = vk::StructureType::eSwapchainCreateInfoKHR;
		createInfo.setSurface(lveDevice.Surface());

		createInfo.setMinImageCount(imageCount);
		createInfo.setImageFormat(surfaceFormat.format);
		createInfo.setImageColorSpace(surfaceFormat.colorSpace);
		createInfo.setImageExtent(extent);
		createInfo.setImageArrayLayers(1);
		createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		QueueFamilyIndices indices              = lveDevice.FindPhysicalQueueFamilies();
		uint32_t           queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

		if (indices.graphicsFamily != indices.presentFamily)
		{
			// Si les familles de files d'attente sont différentes, partagez les images
			createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
			createInfo.setQueueFamilyIndexCount(2);
			createInfo.setPQueueFamilyIndices(queueFamilyIndices);
		}
		else
		{
			// Si les familles de files d'attente sont les mêmes, utilisez exclusivement
			createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices   = nullptr;
		}

		createInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
		createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

		createInfo.setPresentMode(presentMode);
		createInfo.setClipped(VK_TRUE);

		createInfo.setOldSwapchain(oldSwapChain == nullptr ? nullptr : oldSwapChain->swapChain);

		// Crée la chaîne de swap
		swapChain = lveDevice.Device().createSwapchainKHR(createInfo);

		// Obtient les images de la chaîne de swap
		// Récupérer le nombre d'images de swapchain
		swapChainImages = lveDevice.Device().getSwapchainImagesKHR(swapChain);
		swapChainImages.resize(imageCount);

		if (lveDevice.Device().getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages.data()) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to create swap chain!");

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent      = extent;
	}

	void LveSwapChain::CreateImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			vk::ImageViewCreateInfo viewInfo;
			viewInfo.sType                           = vk::StructureType::eImageViewCreateInfo;
			viewInfo.image                           = swapChainImages[i];
			viewInfo.viewType                        = vk::ImageViewType::e2D;
			viewInfo.format                          = swapChainImageFormat;
			viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel   = 0;
			viewInfo.subresourceRange.levelCount     = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount     = 1;

			if (lveDevice.Device().createImageView(&viewInfo, nullptr, &swapChainImageViews[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed!");
		}
	}


	void LveSwapChain::CreateRenderPass()
	{
		vk::AttachmentDescription depthAttachment{};
		depthAttachment.format         = FindDepthFormat();
		depthAttachment.samples        = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout  = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentDescription colorAttachment{};
		colorAttachment.format         = GetSwapChainImageFormat();
		colorAttachment.samples        = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount    = 1;
		subpass.pColorAttachments       = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		vk::SubpassDependency dependency{};
		dependency.dstSubpass    = 0;
		dependency.dstAccessMask =
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		dependency.dstStageMask =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.srcSubpass    = vk::SubpassExternal;
		dependency.srcAccessMask = vk::AccessFlags{};
		dependency.srcStageMask  =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;

		std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		vk::RenderPassCreateInfo                 renderPassInfo{};
		renderPassInfo.sType           = vk::StructureType::eRenderPassCreateInfo;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments    = attachments.data();
		renderPassInfo.subpassCount    = 1;
		renderPassInfo.pSubpasses      = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies   = &dependency;

		if (lveDevice.Device().createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess)
			throw
				std::runtime_error("failed to create render pass!");
	}


	void LveSwapChain::CreateFrameBuffers()
	{
		swapChainFramebuffers.resize(ImageCount());

		for (size_t i = 0; i < ImageCount(); i++)
		{
			std::array<vk::ImageView, 2> attachments = {swapChainImageViews[i], depthImageViews[i]};

			vk::Extent2D              swapChainExtent = GetSwapChainExtent();
			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
			framebufferInfo.setRenderPass(renderPass);
			framebufferInfo.setAttachmentCount(attachments.size());
			framebufferInfo.setPAttachments(attachments.data());
			framebufferInfo.setWidth(swapChainExtent.width);
			framebufferInfo.setHeight(swapChainExtent.height);
			framebufferInfo.setLayers(1);

			if (lveDevice.Device().createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void LveSwapChain::CreateDepthResources()
	{
		vk::Format depthFormat       = FindDepthFormat();
		swapChainDepthFormat         = depthFormat;
		vk::Extent2D swapChainExtent = GetSwapChainExtent();

		depthImages.resize(ImageCount());
		depthImageMemorys.resize(ImageCount());
		depthImageViews.resize(ImageCount());

		for (int i = 0; i < depthImages.size(); i++)
		{
			vk::ImageCreateInfo imageInfo;
			imageInfo.sType = vk::StructureType::eImageCreateInfo;
			imageInfo.setImageType(vk::ImageType::e2D);
			imageInfo.setExtent({swapChainExtent.width, swapChainExtent.height, 1});
			imageInfo.setMipLevels(1);
			imageInfo.setArrayLayers(1);
			imageInfo.setFormat(depthFormat);
			imageInfo.setTiling(vk::ImageTiling::eOptimal);
			imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
			imageInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
			imageInfo.setSamples(vk::SampleCountFlagBits::e1);
			imageInfo.setSharingMode(vk::SharingMode::eExclusive);

			lveDevice.CreateImageWithInfo(
				imageInfo,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				depthImages[i],
				depthImageMemorys[i]);

			vk::ImageViewCreateInfo viewInfo;
			viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
			viewInfo.setImage(depthImages[i]);
			viewInfo.setViewType(vk::ImageViewType::e2D);
			viewInfo.setFormat(depthFormat);
			viewInfo.setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

			if (lveDevice.Device().createImageView(&viewInfo, nullptr, &depthImageViews[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed to create texture image view!");
		}
	}


	void LveSwapChain::CreateSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(ImageCount(), vk::Fence{});

		vk::SemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (lveDevice.Device().createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
			    vk::Result::eSuccess ||
			    lveDevice.Device().createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
			    vk::Result::eSuccess ||
			    lveDevice.Device().createFence(&fenceInfo, nullptr, &inFlightFences[i]) != vk::Result::eSuccess)
				throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	vk::SurfaceFormatKHR LveSwapChain::ChooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
			    availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		}

		return availableFormats[0];
	}


	vk::PresentModeKHR LveSwapChain::ChooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		/* for (const auto& availablePresentMode : availablePresentModes) {
		     if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
		         std::cout << "Present mode: Mailbox" << std::endl;
		         return availablePresentMode;
		     }
		 }*/

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eImmediate)
			{
				std::cout << "Present mode: Immediate" << std::endl;
				return availablePresentMode;
			}
		}

		std::cout << "Present mode: V-Sync" << std::endl;
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D LveSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) return capabilities.currentExtent;
		vk::Extent2D actualExtent = windowExtent;
		actualExtent.width        = std::max(
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

	vk::Format LveSwapChain::FindDepthFormat()
	{
		return lveDevice.FindSupportedFormat(
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}
} // namespace lve
