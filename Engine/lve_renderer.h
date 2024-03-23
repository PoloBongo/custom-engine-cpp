#pragma once

#include "lve_device.h"
#include "lve_swap_chain.h"
#include "lve_window.h"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace lve {
    class LveRenderer {
    public:
        LveRenderer(LveWindow& _window, LveDevice& _device);
        ~LveRenderer();

        LveRenderer(const LveRenderer&) = delete;
        LveRenderer& operator=(const LveRenderer&) = delete;

        VkRenderPass GetSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
        float GetAspectRatio() const { return lveSwapChain->extentAspectRatio(); }
        bool IsFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer GetCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int GetFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer _commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer _commandBuffer);

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        LveWindow& lveWindow;
        LveDevice& lveDevice;
        std::unique_ptr<LveSwapChain> lveSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex = 0;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;
    };
}  // namespace lve