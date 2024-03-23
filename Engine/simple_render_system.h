#pragma once

#include "lve_camera.h"
#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_game_object.h"

//std
#include <memory>
#include <vector>

namespace lve {

    struct SimplePushConstantData {
        glm::mat4 transform{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
        
    };

    /**
     * @brief Classe représentant la première application utilisant Vulkan.
     *
     * Cette classe gère l'exécution de la première application Vulkan, incluant la création de la fenêtre.
     */
    class SimpleRenderSystem {
    public:

        SimpleRenderSystem(LveDevice& _device, VkRenderPass _renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete;


        void RenderGameObjects(VkCommandBuffer _commandBuffer, std::vector<LveGameObject> &g_ameObjects, const LveCamera& _camera);

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass _renderPass);

        LveDevice &lveDevice;


        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };

} // namespace lve
