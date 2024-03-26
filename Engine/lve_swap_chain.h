#pragma once

#include "lve_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace lve {

    class LveSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        LveSwapChain(LveDevice& deviceRef, VkExtent2D windowExtent);
        LveSwapChain(LveDevice& deviceRef, VkExtent2D extent, std::shared_ptr<LveSwapChain> previous);
        ~LveSwapChain();
        void Init();

        LveSwapChain(const LveSwapChain&) = delete;
        LveSwapChain &operator=(const LveSwapChain&) = delete;

        /**
         * @brief Obtient le framebuffer correspondant à l'indice spécifié.
         *
         * Cette fonction retourne le framebuffer de la swap chain qui correspond à l'indice spécifié.
         *
         * @param index L'indice du framebuffer à récupérer.
         * @return VkFramebuffer Le framebuffer correspondant à l'indice spécifié.
         */
        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }

        /**
         * @brief Obtient le passe de rendu.
         *
         * Cette fonction retourne le passe de rendu associé à la swap chain.
         *
         * @return VkRenderPass Le passe de rendu associé à la swap chain.
         */
        VkRenderPass getRenderPass() { return renderPass; }

        /**
         * @brief Obtient la vue d'image correspondant à l'indice spécifié.
         *
         * Cette fonction retourne la vue d'image de la swap chain qui correspond à l'indice spécifié.
         *
         * @param index L'indice de la vue d'image à récupérer.
         * @return VkImageView La vue d'image correspondant à l'indice spécifié.
         */
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }

        /**
         * @brief Obtient le nombre d'images dans la swap chain.
         *
         * Cette fonction retourne le nombre d'images actuellement présentes dans la swap chain.
         *
         * @return size_t Le nombre d'images dans la swap chain.
         */
        size_t imageCount() { return swapChainImages.size(); }

        /**
         * @brief Obtient le format d'image de la swap chain.
         *
         * Cette fonction retourne le format d'image utilisé par la swap chain.
         *
         * @return VkFormat Le format d'image de la swap chain.
         */
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }

        /**
         * @brief Obtient l'étendue de la swap chain.
         *
         * Cette fonction retourne l'étendue (largeur et hauteur) de la swap chain.
         *
         * @return VkExtent2D L'étendue de la swap chain.
         */
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }

        /**
         * @brief Obtient la largeur de la swap chain.
         *
         * Cette fonction retourne la largeur de la swap chain.
         *
         * @return uint32_t La largeur de la swap chain.
         */
        uint32_t width() { return swapChainExtent.width; }

        /**
         * @brief Obtient la hauteur de la swap chain.
         *
         * Cette fonction retourne la hauteur de la swap chain.
         *
         * @return uint32_t La hauteur de la swap chain.
         */
        uint32_t height() { return swapChainExtent.height; }

        /**
         * @brief Calcule le rapport d'aspect de l'étendue de la swap chain.
         *
         * Cette fonction calcule le rapport d'aspect (largeur/hauteur) de l'étendue de la swap chain.
         *
         * @return float Le rapport d'aspect de l'étendue de la swap chain.
         */
        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        /**
         * @brief Recherche le format de profondeur adapté.
         *
         * Cette fonction recherche le format de profondeur adapté parmi une liste de formats donnée, en tenant compte des caractéristiques spécifiées.
         *
         * @return VkFormat Le format de profondeur adapté.
         */
        VkFormat findDepthFormat();

        /**
         * @brief Acquiert l'index de l'image suivante dans la chaîne de swaps.
         *
         * Cette fonction acquiert l'index de l'image suivante dans la chaîne de swaps.
         *
         * @param imageIndex Pointeur vers la variable où stocker l'index de l'image acquise.
         * @return VkResult Le résultat de l'opération.
         */
        VkResult acquireNextImage(uint32_t* imageIndex);

        /**
         * @brief Soumet les command buffers pour exécution et présente le résultat.
         *
         * Cette fonction soumet les command buffers spécifiés pour exécution, attend leur achèvement, puis présente le résultat à l'écran.
         *
         * @param buffers Tableau des command buffers à soumettre.
         * @param imageIndex Pointeur vers l'index de l'image à présenter.
         * @return VkResult Le résultat de l'opération.
         */
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

        /**
         * @brief Compare les formats de profondeur et d'image avec une autre LveSwapChain.
         *
         * Cette fonction compare les formats de profondeur et d'image de cette LveSwapChain avec ceux d'une autre LveSwapChain spécifiée.
         *
         * @param _swapChain La LveSwapChain avec laquelle comparer les formats.
         * @return bool True si les formats de profondeur et d'image sont identiques, sinon False.
         */
        bool CompareSwapFormats(const LveSwapChain& _swapChain) const {
            return  _swapChain.swapChainDepthFormat == swapChainDepthFormat && 
                    _swapChain.swapChainImageFormat == swapChainImageFormat;
        }

    private:

        /**
         * @brief Crée la chaîne de swaps.
         *
         * Cette fonction crée la chaîne de swaps, qui est utilisée pour la présentation des images à l'écran.
         * La création de la chaîne de swaps implique la sélection des formats de surface, des modes de présentation et des dimensions appropriés, ainsi que la configuration de divers autres paramètres liés à la présentation.
         *
         * @throws std::runtime_error si la création de la chaîne de swaps échoue.
         */
        void createSwapChain();

        /**
         * @brief Crée les vues d'image de la chaîne de swaps.
         *
         * Cette fonction crée les vues d'image pour chaque image de la chaîne de swaps.
         * Chaque vue d'image est associée à une image de la chaîne de swaps et spécifie comment cette image doit être interprétée lorsqu'elle est utilisée comme texture.
         *
         * @throws std::runtime_error si la création d'une vue d'image échoue.
         */
        void createImageViews();

        /**
        * @brief Crée les ressources de profondeur pour la chaîne de swaps.
        *
        * Cette fonction crée les ressources de profondeur nécessaires pour chaque image de la chaîne de swaps.
        * Ces ressources de profondeur sont utilisées pour le rendu des scènes en 3D afin de déterminer la profondeur des fragments et gérer la détection des collisions.
        *
        * @throws std::runtime_error si la création d'une ressource de profondeur échoue.
        */
        void createDepthResources();

        /**
         * @brief Crée la passe de rendu.
         *
         * Cette fonction crée la passe de rendu utilisée pour définir comment les données de géométrie sont traitées lors du rendu d'une image.
         * La passe de rendu définit les attachements de couleur et de profondeur, ainsi que les sous-passes qui décrivent les opérations de rendu à exécuter.
         *
         * @throws std::runtime_error si la création de la passe de rendu échoue.
         */
        void createRenderPass();

        /**
         * @brief Crée les tampons de trame.
         *
         * Cette fonction crée les tampons de trame qui sont utilisés pour stocker les informations de rendu associées à chaque image de la chaîne de swaps.
         * Chaque tampon de trame est associé à une image de la chaîne de swaps et est configuré avec les vues d'image et les profondeurs appropriées pour le rendu.
         *
         * @throws std::runtime_error si la création d'un tampon de trame échoue.
         */
        void createFramebuffers();

        /**
         * @brief Crée les objets de synchronisation.
         *
         * Cette fonction crée les objets de synchronisation utilisés pour synchroniser l'exécution des opérations de rendu avec la présentation des images.
         * Ces objets comprennent des sémaphores pour la synchronisation entre les différentes étapes du pipeline graphique, ainsi que des clôtures de barrière pour la synchronisation entre le CPU et le GPU.
         *
         * @throws std::runtime_error si la création d'un objet de synchronisation échoue.
         */
        void createSyncObjects();

        // Helper functions

        /**
         * @brief Choisissez le format de surface de la chaîne de swaps.
         *
         * Cette fonction choisit le format de surface de la chaîne de swaps en parcourant les formats disponibles et en recherchant un format spécifique (B8G8R8A8_SRGB) avec un espace colorimétrique compatible (VK_COLOR_SPACE_SRGB_NONLINEAR_KHR).
         * Si un tel format est trouvé, il est renvoyé. Sinon, le premier format disponible est renvoyé par défaut.
         *
         * @param availableFormats Les formats de surface disponibles.
         * @return Le format de surface choisi.
         */
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);

        /**
         * @brief Choisissez le mode de présentation de la chaîne de swaps.
         *
         * Cette fonction choisit le mode de présentation de la chaîne de swaps en parcourant les modes de présentation disponibles et en recherchant un mode spécifique (VK_PRESENT_MODE_IMMEDIATE_KHR) qui offre une présentation immédiate sans attente de synchronisation verticale (V-Sync).
         * Si un tel mode est trouvé, il est renvoyé. Sinon, le mode de présentation FIFO (VK_PRESENT_MODE_FIFO_KHR) avec synchronisation verticale est renvoyé par défaut.
         *
         * @param availablePresentModes Les modes de présentation disponibles.
         * @return Le mode de présentation choisi.
         */
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);

        /**
         * @brief Choisissez l'étendue de la chaîne de swaps.
         *
         * Cette fonction choisit l'étendue de la chaîne de swaps en se basant sur les capacités de la surface de rendu.
         * Si l'étendue actuelle est déjà définie dans les capacités de la surface, elle est retournée directement.
         * Sinon, une étendue appropriée est calculée en fonction de la taille de la fenêtre et des contraintes minimales et maximales spécifiées dans les capacités de la surface.
         *
         * @param capabilities Les capacités de la surface de rendu.
         * @return L'étendue de la chaîne de swaps choisie.
         */
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        LveDevice& device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<LveSwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace lve
