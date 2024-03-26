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

        /**
         * @brief Obtient l'objet de rendu associé à la chaîne de swap.
         *
         * Cette fonction retourne l'objet de rendu associé à la chaîne de swap.
         *
         * @return VkRenderPass L'objet de rendu associé à la chaîne de swap.
         */
        VkRenderPass GetSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }

        /**
         * @brief Obtient le ratio de l'aspect de la fenêtre associée à la chaîne de swap.
         *
         * Cette fonction retourne le ratio de l'aspect de la fenêtre associée à la chaîne de swap.
         *
         * @return float Le ratio de l'aspect de la fenêtre.
         */
        float GetAspectRatio() const { return lveSwapChain->extentAspectRatio(); }

        /**
         * @brief Vérifie si un cadre est en cours.
         *
         * Cette fonction retourne vrai si un cadre est actuellement en cours, sinon elle retourne faux.
         *
         * @return bool Vrai si un cadre est en cours, sinon faux.
         */
        bool IsFrameInProgress() const { return isFrameStarted; }

        /**
         * @brief Obtient le tampon de commandes actuel.
         *
         * Cette fonction retourne le tampon de commandes actuel. Elle suppose qu'un cadre est en cours.
         *
         * @return VkCommandBuffer Le tampon de commandes actuel.
         * @throws std::runtime_error si aucun cadre n'est en cours.
         */
        VkCommandBuffer GetCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        /**
         * @brief Obtient l'index du cadre en cours.
         *
         * Cette fonction retourne l'index du cadre en cours. Elle suppose qu'un cadre est en cours.
         *
         * @return int L'index du cadre en cours.
         * @throws std::runtime_error si aucun cadre n'est en cours.
         */
        int GetFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        /**
         * @brief Démarre un nouveau cadre.
         *
         * Cette fonction démarre un nouveau cadre en acquérant l'image suivante de la chaîne de swaps.
         *
         * @return VkCommandBuffer Le tampon de commandes actuel.
         * @throws std::runtime_error si l'acquisition de l'image échoue ou si le tampon de commandes ne peut pas être commencé.
         */
        VkCommandBuffer BeginFrame();

        /**
         * @brief Termine le cadre en cours.
         *
         * Cette fonction termine le cadre en cours en arrêtant l'enregistrement du tampon de commandes, en soumettant
         * le tampon de commandes à la chaîne de swaps pour présentation et en gérant les événements de redimensionnement de la fenêtre.
         *
         * @throws std::runtime_error si l'arrêt de l'enregistrement du tampon de commandes échoue, si la présentation de l'image de la chaîne de swaps échoue
         * ou si une redimensionnement de la fenêtre est nécessaire.
         */
        void EndFrame();

        /**
         * @brief Démarre la passe de rendu de la chaîne de swaps.
         *
         * Cette fonction démarre la passe de rendu de la chaîne de swaps en commençant par initialiser
         * les informations sur le rendu, en définissant la zone de rendu et en définissant les valeurs de nettoyage.
         *
         * @param commandBuffer Le tampon de commandes sur lequel commencer la passe de rendu.
         *
         * @throws std::runtime_error si la commande de début de la passe de rendu échoue.
         */
        void BeginSwapChainRenderPass(VkCommandBuffer _commandBuffer);

        /**
         * @brief Termine la passe de rendu de la chaîne de swaps.
         *
         * Cette fonction termine la passe de rendu de la chaîne de swaps en appelant la commande de fin
         * de la passe de rendu sur le tampon de commandes spécifié.
         *
         * @param commandBuffer Le tampon de commandes sur lequel terminer la passe de rendu.
         */
        void EndSwapChainRenderPass(VkCommandBuffer _commandBuffer);

    private:

        /**
         * @brief Crée les tampons de commandes.
         *
         * Cette méthode alloue les tampons de commandes pour chaque image de la chaîne de swaps.
         *
         * @throw std::runtime_error si l'allocation des tampons de commandes échoue.
         */
        void CreateCommandBuffers();

        /**
         * @brief Libère les tampons de commandes.
         *
         * Cette méthode libère les tampons de commandes alloués précédemment.
         */
        void FreeCommandBuffers();

        /**
         * @brief Recreate la chaîne de swaps.
         *
         * Cette méthode recrée la chaîne de swaps en fonction de la nouvelle étendue de la fenêtre.
         * Elle attend également que la fenêtre ait une étendue valide avant de procéder à la recréation.
         * Si une chaîne de swaps existait déjà, elle est détruite et remplacée par la nouvelle chaîne de swaps.
         * Si les formats d'image (ou de profondeur) de la nouvelle chaîne de swaps diffèrent de ceux de l'ancienne,
         * une erreur est levée.
         */
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