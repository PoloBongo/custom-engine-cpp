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

        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size_t imageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32_t width() { return swapChainExtent.width; }
        uint32_t height() { return swapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

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
