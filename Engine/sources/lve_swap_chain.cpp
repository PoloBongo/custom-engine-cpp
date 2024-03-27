#include "lve_swap_chain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace lve {


    LveSwapChain::LveSwapChain(LveDevice& deviceRef, vk::Extent2D extent)
        : device{ deviceRef }, windowExtent{ extent } {
        Init();
    }

    LveSwapChain::LveSwapChain(LveDevice& deviceRef, vk::Extent2D extent, std::shared_ptr<LveSwapChain> previous) : device{ deviceRef }, windowExtent{ extent }, oldSwapChain{ previous } {
        Init();
        oldSwapChain = nullptr;
    }

    void LveSwapChain:: Init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }


    LveSwapChain::~LveSwapChain() {
        // Nettoie les image views de la chaîne de swap
        for (auto imageView : swapChainImageViews) {
            device.device().destroyImageView(imageView);
        }
        swapChainImageViews.clear();

        // Détruit la chaîne de swap
        if (swapChain != nullptr) {
            device.device().destroySwapchainKHR(swapChain);
            swapChain = nullptr;
        }

        // Nettoie les images de profondeur, leurs vues et la mémoire associée
        for (int i = 0; i < depthImages.size(); i++) {
            device.device().destroyImageView(depthImageViews[i]);
            device.device().destroyImage(depthImages[i]);
            device.device().freeMemory(depthImageMemorys[i]);
        }

        // Nettoie les framebuffers de la chaîne de swap
        for (auto framebuffer : swapChainFramebuffers) {
            device.device().destroyFramebuffer(framebuffer);
        }

        // Détruit le render pass
        device.device().destroyRenderPass(renderPass);

        // Nettoie les objets de synchronisation
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.device().destroySemaphore(renderFinishedSemaphores[i]);
            device.device().destroySemaphore(imageAvailableSemaphores[i]);
            device.device().destroyFence(inFlightFences[i]);
        }
    }

    vk::Result LveSwapChain::acquireNextImage(uint32_t* imageIndex) {
        // Attend que les barrières de synchronisation soient satisfaites
        device.device().waitForFences(1, &inFlightFences[currentFrame], true, std::numeric_limits<uint64_t>::max());

        // Acquiert l'index de l'image suivante dans la chaîne de swap
        // en utilisant l'extension KHR_swapchain
        vk::Result result = device.device().acquireNextImageKHR(
            swapChain,                                          // La chaîne de swap
            std::numeric_limits<uint64_t>::max(),              // Durée d'attente infinie
            imageAvailableSemaphores[currentFrame],            // Sémaphore non signalé (attendre qu'il soit signalé avant)
            nullptr,                                           // Pas de gestion de l'objet de barrière de blocage
            imageIndex);                                       // L'index de l'image acquise

        return result;
    }

    vk::Result LveSwapChain::submitCommandBuffers(
        const vk::CommandBuffer* buffers, uint32_t* imageIndex) {
        // Attend la fin des commandes en cours d'exécution pour l'image en cours d'acquisition
        if (imagesInFlight[*imageIndex] != vk::Fence()) {
            device.device().waitForFences(imagesInFlight[*imageIndex], true, std::numeric_limits<uint64_t>::max());
        }
        imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

        // Configuration de la soumission des commandes
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

        // Soumet les commandes à la file de commandes graphiques
        device.device().resetFences(inFlightFences[currentFrame]);
        device.graphicsQueue().submit(submitInfo, inFlightFences[currentFrame]);

        // Configuration de la présentation
        vk::PresentInfoKHR presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = imageIndex;

        // Présente l'image
        vk::Result result = device.presentQueue().presentKHR(presentInfo);

        // Passe au prochain frame
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }



    void LveSwapChain::createSwapChain() {
        // Obtient les détails de prise en charge de la chaîne de swap
        SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

        // Choix du format de surface de la chaîne de swap
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

        // Choix du mode de présentation de la chaîne de swap
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

        // Choix de l'étendue de la chaîne de swap
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // Détermine le nombre d'images dans la chaîne de swap
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        // Configuration de la création de la chaîne de swap
        vk::SwapchainCreateInfoKHR createInfo = {};
        createInfo.setSurface(device.surface());

        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

        if (indices.graphicsFamily != indices.presentFamily) {
            // Si les familles de files d'attente sont différentes, partagez les images
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setPQueueFamilyIndices(queueFamilyIndices);
        }
        else {
            // Si les familles de files d'attente sont les mêmes, utilisez exclusivement
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        }

        createInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
        createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

        createInfo.setPresentMode(presentMode);
        createInfo.setClipped(true);

        createInfo.setOldSwapchain(oldSwapChain == nullptr ? nullptr : oldSwapChain->swapChain);

        // Crée la chaîne de swap
        swapChain = device.device().createSwapchainKHR(createInfo);

        // Obtient les images de la chaîne de swap
        // Récupérer le nombre d'images de swapchain
        uint32_t actualImageCount = imageCount;
        swapChainImages = device.device().getSwapchainImagesKHR(swapChain);

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void LveSwapChain::createRenderPass() {
        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = getSwapChainImageFormat();
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = vk::SubpassExternal;
        dependency.srcAccessMask = vk::AccessFlags{};
        dependency.srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask =
            vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        renderPass = device.device().createRenderPass(renderPassInfo);
        if (!renderPass) {
            throw std::runtime_error("failed to create render pass!");
        }
    }


    void LveSwapChain::createFramebuffers() {
        swapChainFramebuffers.resize(imageCount());
        for (size_t i = 0; i < imageCount(); i++) {
            std::array<vk::ImageView, 2> attachments = { swapChainImageViews[i], depthImageViews[i] };

            vk::Extent2D swapChainExtent = getSwapChainExtent();
            vk::FramebufferCreateInfo framebufferInfo;
            framebufferInfo.setRenderPass(renderPass);
            framebufferInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
            framebufferInfo.setPAttachments(attachments.data());
            framebufferInfo.setWidth(swapChainExtent.width);
            framebufferInfo.setHeight(swapChainExtent.height);
            framebufferInfo.setLayers(1);

            try {
                swapChainFramebuffers[i] = device.device().createFramebuffer(framebufferInfo);
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error(std::string("failed to create framebuffer!") + e.what());
            }
        }
    }

    void LveSwapChain::createDepthResources() {
        vk::Format depthFormat = findDepthFormat();
        swapChainDepthFormat = depthFormat;
        vk::Extent2D swapChainExtent = getSwapChainExtent();

        depthImages.resize(imageCount());
        depthImageMemorys.resize(imageCount());
        depthImageViews.resize(imageCount());

        for (int i = 0; i < depthImages.size(); i++) {
            vk::ImageCreateInfo imageInfo;
            imageInfo.setImageType(vk::ImageType::e2D);
            imageInfo.setExtent({ swapChainExtent.width, swapChainExtent.height, 1 });
            imageInfo.setMipLevels(1);
            imageInfo.setArrayLayers(1);
            imageInfo.setFormat(depthFormat);
            imageInfo.setTiling(vk::ImageTiling::eOptimal);
            imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
            imageInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
            imageInfo.setSamples(vk::SampleCountFlagBits::e1);
            imageInfo.setSharingMode(vk::SharingMode::eExclusive);

            device.createImageWithInfo(
                imageInfo,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                depthImages[i],
                depthImageMemorys[i]);

            vk::ImageViewCreateInfo viewInfo;
            viewInfo.setImage(depthImages[i]);
            viewInfo.setViewType(vk::ImageViewType::e2D);
            viewInfo.setFormat(depthFormat);
            viewInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 });

            try {
                depthImageViews[i] = device.device().createImageView(viewInfo);
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error(std::string("failed to create depth image view!") + e.what());
            }
        }
    }


    void LveSwapChain::createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(imageCount(), vk::Fence{});

        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

        vk::FenceCreateInfo fenceInfo = {};
        fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            try {
                imageAvailableSemaphores[i] = device.device().createSemaphore(semaphoreInfo);
                renderFinishedSemaphores[i] = device.device().createSemaphore(semaphoreInfo);
                inFlightFences[i] = device.device().createFence(fenceInfo);
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error(std::string("failed to create synchronization objects for a frame: ") + e.what());
            }
        }
    }



    vk::SurfaceFormatKHR LveSwapChain::chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }


    vk::PresentModeKHR LveSwapChain::chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR>& availablePresentModes) {
       /* for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                std::cout << "Present mode: Mailbox" << std::endl;
                return availablePresentMode;
            }
        }*/

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
                std::cout << "Present mode: Immediate" << std::endl;
                return availablePresentMode;
            }
        }

        std::cout << "Present mode: V-Sync" << std::endl;
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D LveSwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            vk::Extent2D actualExtent = windowExtent;
            actualExtent.width = std::max(
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    vk::Format LveSwapChain::findDepthFormat() {
        return device.findSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

}  // namespace lve
