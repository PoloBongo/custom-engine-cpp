#include "lve_renderer.h"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

    LveRenderer::LveRenderer(LveWindow& window, LveDevice& device)
        : lveWindow{ window }, lveDevice{ device } {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    LveRenderer::~LveRenderer() { FreeCommandBuffers(); }

    void LveRenderer::RecreateSwapChain() {
        auto extent = lveWindow.GetExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lveWindow.GetExtent();
            glfwWaitEvents();
        }

        lveDevice.device().waitIdle(); // Utilisation de vk::Device::waitIdle()

        if (lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        }
        else {
            auto oldSwapChain = std::move(lveSwapChain);
            std::shared_ptr<LveSwapChain> sharedOldSwapChain = std::move(oldSwapChain);
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, sharedOldSwapChain);

            if (!oldSwapChain->CompareSwapFormats(*lveSwapChain)) {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }


    void LveRenderer::CreateCommandBuffers() {
        commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        try {
            commandBuffers = lveDevice.device().allocateCommandBuffers(allocInfo);
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }


    void LveRenderer::FreeCommandBuffers() {
        lveDevice.device().freeCommandBuffers(
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }


    vk::CommandBuffer LveRenderer::BeginFrame() {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
            RecreateSwapChain();
            return nullptr;
        }
        else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = GetCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);
        return commandBuffer;
    }




    void LveRenderer::EndFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

        auto commandBuffer = GetCurrentCommandBuffer();
        commandBuffer.end();

        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
            lveWindow.WasWindowResized()) {
            lveWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
        else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
    }


    void LveRenderer::BeginSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(
            commandBuffer == GetCurrentCommandBuffer() &&
            "Can't begin render pass on command buffer from a different frame");

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<vk::ClearValue, 2> clearValues{};
        clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{0.01f, 0.01f, 0.01f, 1.0f});
        clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
        commandBuffer.setViewport(0, viewport);
        commandBuffer.setScissor(0, scissor);
    }


    void LveRenderer::EndSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() &&
            "Can't end render pass on command buffer from a different frame");
        commandBuffer.endRenderPass();
    }

}  // namespace lve