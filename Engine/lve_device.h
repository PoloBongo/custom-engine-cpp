#pragma once

#include "lve_window.h"

// std lib headers
#include <string>
#include <vector>

namespace lve {

    // Structure pour stocker les détails de support de la chaîne d'échange
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities; // Capacités de la surface Vulkan
        std::vector<VkSurfaceFormatKHR> formats; // Formats de surface Vulkan pris en charge
        std::vector<VkPresentModeKHR> presentModes; // Modes de présentation Vulkan pris en charge
    };

    // Structure pour stocker les indices des files d'attente requises
    struct QueueFamilyIndices {
        uint32_t graphicsFamily; // Indice de la famille de files d'attente graphiques
        uint32_t presentFamily; // Indice de la famille de files d'attente de présentation
        bool graphicsFamilyHasValue = false; // Indique si l'indice de la famille de files d'attente graphiques est défini
        bool presentFamilyHasValue = false; // Indique si l'indice de la famille de files d'attente de présentation est défini

        // Vérifie si les indices des files d'attente requis sont complets
        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class LveDevice {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        /**
        * @brief Constructeur de la classe LveDevice.
        *
        * Initialise un périphérique Vulkan en prenant une référence à une fenêtre Vulkan LveWindow en tant que paramètre.
        *
        * @param window Une référence à la fenêtre Vulkan utilisée pour initialiser le périphérique.
        */
        LveDevice(LveWindow& window);

        /**
         * @brief Destructeur de la classe LveDevice.
         *
         * Nettoie les ressources associées au périphérique Vulkan lorsqu'il est détruit.
         */
        ~LveDevice();

        // Not copyable or movable
        /**
        * @brief Constructeur de copie supprimé.
        *
        * Empêche la création d'une nouvelle instance de LveDevice en copiant une instance existante.
        */
        LveDevice(const LveDevice&) = delete;

        /**
        * @brief Opérateur d'affectation de copie supprimé.
        *
        * Empêche la copie des membres d'une instance de LveDevice vers une autre instance existante.
        */
        void operator=(const LveDevice&) = delete;

        /**
        * @brief Constructeur de déplacement supprimé.
        *
        * Empêche la création d'une nouvelle instance de LveDevice en déplaçant une instance existante.
        */
        LveDevice(LveDevice&&) = delete;

        /**
        * @brief Opérateur d'affectation de déplacement supprimé.
        *
        * Empêche le déplacement des membres d'une instance de LveDevice vers une autre instance existante.
        */
        LveDevice& operator=(LveDevice&&) = delete;

        /**
        * @brief Récupère le pool de commandes Vulkan associé au périphérique.
        *
        * @return Le pool de commandes Vulkan associé au périphérique.
        */
        VkCommandPool getCommandPool() { return commandPool; }

        /**
         * @brief Récupère l'objet de périphérique Vulkan.
         *
         * @return L'objet de périphérique Vulkan.
         */
        VkDevice device() { return device_; }

        /**
         * @brief Récupère la surface Vulkan associée au périphérique.
         *
         * @return La surface Vulkan associée au périphérique.
         */
        VkSurfaceKHR surface() { return surface_; }

        /**
         * @brief Récupère la file de commandes graphiques du périphérique.
         *
         * @return La file de commandes graphiques du périphérique.
         */
        VkQueue graphicsQueue() { return graphicsQueue_; }

        /**
         * @brief Récupère la file de commandes de présentation du périphérique.
         *
         * @return La file de commandes de présentation du périphérique.
         */
        VkQueue presentQueue() { return presentQueue_; }


        /**
         * @brief Récupère les détails de prise en charge de la chaîne de balisage pour le périphérique.
         *
         * @return Les détails de prise en charge de la chaîne de balisage pour le périphérique.
         */
        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

        /**
         * @brief Trouve un type de mémoire approprié pour une utilisation spécifiée.
         *
         * @param typeFilter Le type de filtre de mémoire.
         * @param properties Les propriétés de la mémoire.
         * @return Le type de mémoire approprié.
         */
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        /**
         * @brief Trouve les familles de files de commandes physiques disponibles sur le périphérique.
         *
         * @return Les familles de files de commandes physiques disponibles sur le périphérique.
         */
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

        /**
         * @brief Trouve un format d'image supporté par le périphérique.
         *
         * @param candidates Les formats d'image candidats.
         * @param tiling L'inclinaison de l'image.
         * @param features Les fonctionnalités du format de l'image.
         * @return Le format d'image supporté par le périphérique.
         */
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


        // Buffer Helper Functions
        /**
         * @brief Crée un tampon Vulkan avec les paramètres spécifiés.
         *
         * @param size La taille du tampon.
         * @param usage Les indicateurs d'utilisation du tampon.
         * @param properties Les propriétés de la mémoire du tampon.
         * @param buffer Référence à l'objet tampon Vulkan créé.
         * @param bufferMemory Référence à l'objet mémoire du tampon Vulkan créé.
         */
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        /**
         * @brief Démarre une séquence de commandes Vulkan temporaires.
         *
         * Cette fonction alloue un tampon de commandes Vulkan et le démarre pour une utilisation unique.
         *
         * @return Le tampon de commandes Vulkan alloué.
         */
        VkCommandBuffer beginSingleTimeCommands();

        /**
         * @brief Termine une séquence de commandes Vulkan temporaires.
         *
         * Cette fonction termine l'exécution d'une séquence de commandes Vulkan temporaires et libère les ressources associées.
         *
         * @param commandBuffer Le tampon de commandes Vulkan à terminer.
         */
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        /**
         * @brief Copie les données d'un tampon source vers un tampon de destination.
         *
         * @param srcBuffer Le tampon source à partir duquel copier les données.
         * @param dstBuffer Le tampon de destination où copier les données.
         * @param size La taille des données à copier.
         */
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory);

        VkPhysicalDeviceProperties properties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        LveWindow& window;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}  // namespace lve