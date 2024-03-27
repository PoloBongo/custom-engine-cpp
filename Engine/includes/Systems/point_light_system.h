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

    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    /**
     * @brief Classe représentant la première application utilisant Vulkan.
     *
     * Cette classe gère l'exécution de la première application Vulkan, incluant la création de la fenêtre.
     */
    class PointLightSystem {
    public:

        PointLightSystem(LveDevice& _device, VkRenderPass _renderPass, VkDescriptorSetLayout _globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem operator=(const PointLightSystem&) = delete;

        /**
         * @brief Rendu du système de lumières ponctuelles.
         *
         * Cette méthode effectue le rendu du système de lumières ponctuelles en utilisant le pipeline de rendu spécifié.
         * Elle lie également les ensembles de descripteurs globaux nécessaires pour le rendu.
         * Enfin, elle utilise une commande de dessin pour rendre les lumières ponctuelles.
         *
         * @param _frameInfo Informations sur le frame actuel, contenant notamment le tampon de commandes.
         */
        void Update(FrameInfo &_frameInfo, GlobalUbo& _ubo);
        void Render(FrameInfo &_frameInfo);

    private:

        /**
         * @brief Crée le layout du pipeline pour le système de lumières ponctuelles.
         *
         * Cette méthode crée le layout du pipeline pour le système de lumières ponctuelles en utilisant
         * le layout du jeu de descripteurs globaux spécifié.
         *
         * @param _globalSetLayout Layout du jeu de descripteurs globaux utilisé dans le pipeline.
         */
        void CreatePipelineLayout(VkDescriptorSetLayout _globalSetLayout);

        /**
        * @brief Crée le pipeline pour le système de lumières ponctuelles.
        *
        * Cette méthode crée le pipeline pour le système de lumières ponctuelles en utilisant le passe de rendu
        * spécifié et le layout du pipeline précédemment créé.
        *
        * @param _renderPass Le passe de rendu auquel ce pipeline est lié.
        * @throws std::runtime_error Si la création du pipeline échoue.
        */
        void CreatePipeline(VkRenderPass _renderPass);

        LveDevice &lveDevice;


        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };

} // namespace lve
