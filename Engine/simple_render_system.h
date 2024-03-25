#pragma once

#include "lve_camera.h"
#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_frame_info.h"

//std
#include <memory>
#include <vector>

namespace lve {

    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
        
    };

    /**
     * @brief Classe représentant la première application utilisant Vulkan.
     *
     * Cette classe gère l'exécution de la première application Vulkan, incluant la création de la fenêtre.
     */
    class SimpleRenderSystem {
    public:

        SimpleRenderSystem(LveDevice& _device, VkRenderPass _renderPass, VkDescriptorSetLayout _globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete;


        void RenderGameObjects(FrameInfo &_frameInfo);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout _globalSetLayout);
        void CreatePipeline(VkRenderPass _renderPass);

        LveDevice &lveDevice;


        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };

} // namespace lve
