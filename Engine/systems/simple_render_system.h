#pragma once

#include "../lve_camera.h"
#include "../lve_pipeline.h"
#include "../lve_device.h"
#include "../lve_game_object.h"
#include "../lve_frame_info.h"

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

        /**
         * @brief Rend les objets de jeu avec le système de rendu simple.
         *
         * Cette méthode rend les objets de jeu avec le système de rendu simple en utilisant le pipeline graphique
         * et le layout du pipeline spécifiés dans le cadre d'une Frame.
         *
         * @param _frameInfo Les informations de frame, y compris le tampon de commandes, le descripteur global, et les objets de jeu.
         */
        void RenderGameObjects(FrameInfo &_frameInfo);

    private:

        /**
         * @brief Crée le layout du pipeline pour le système de rendu simple.
         *
         * Cette méthode crée le layout du pipeline pour le système de rendu simple en spécifiant le layout du descripteur global
         * ainsi que la plage des constantes poussées utilisées par les shaders.
         *
         * @param _globalSetLayout Le layout du descripteur global.
         */
        void CreatePipelineLayout(VkDescriptorSetLayout _globalSetLayout);

        /**
         * @brief Crée le pipeline pour le système de rendu simple.
         *
         * Cette méthode crée le pipeline pour le système de rendu simple en utilisant les shaders spécifiés et le layout de pipeline
         * préalablement créé. Le rendu se fait selon le render pass spécifié.
         *
         * @param _renderPass Le render pass utilisé pour le rendu.
         */
        void CreatePipeline(VkRenderPass _renderPass);

        LveDevice &lveDevice;


        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };

} // namespace lve
