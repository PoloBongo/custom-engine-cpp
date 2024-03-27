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
        LveDevice &operator=(const LveDevice&) = delete;

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

        /**
         * @brief Copie les données d'un tampon vers une image Vulkan.
         *
         * Cette fonction copie les données d'un tampon Vulkan vers une image Vulkan en spécifiant la largeur, la hauteur et le nombre de couches de l'image.
         *
         * @param buffer Le tampon contenant les données à copier.
         * @param image L'image Vulkan de destination.
         * @param width La largeur de l'image.
         * @param height La hauteur de l'image.
         * @param layerCount Le nombre de couches de l'image.
         */
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        /**
         * @brief Crée une image Vulkan avec les informations fournies.
         *
         * Cette fonction crée une image Vulkan en utilisant les informations spécifiées dans la structure VkImageCreateInfo.
         *
         * @param imageInfo Les informations de création de l'image.
         * @param properties Les propriétés de la mémoire de l'image.
         * @param image Référence à l'objet image Vulkan créé.
         * @param imageMemory Référence à l'objet mémoire de l'image Vulkan créé.
         */
        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory);

        /**
         * @brief Propriétés du périphérique physique Vulkan associé.
         *
         * Cette structure contient les propriétés du périphérique physique Vulkan associé.
         */
        VkPhysicalDeviceProperties properties;


    private:

        /**
         * @brief Crée une instance Vulkan.
         *
         * Cette fonction initialise une instance Vulkan, qui représente la connexion entre l'application et l'API Vulkan.
         */
        void createInstance();

        /**
         * @brief Configure le gestionnaire de débogage.
         *
         * Cette fonction configure un gestionnaire de débogage Vulkan pour recevoir les messages de validation et de débogage de l'API Vulkan.
         */
        void setupDebugMessenger();

        /**
         * @brief Crée une surface Vulkan.
         *
         * Cette fonction crée une surface Vulkan à partir de la fenêtre Vulkan associée.
         */
        void createSurface();

        /**
         * @brief Sélectionne le périphérique physique Vulkan approprié.
         *
         * Cette fonction sélectionne le périphérique physique Vulkan approprié parmi ceux disponibles sur le système.
         */
        void pickPhysicalDevice();

        /**
         * @brief Crée le périphérique logique Vulkan.
         *
         * Cette fonction crée le périphérique logique Vulkan qui sera utilisé pour interagir avec le matériel graphique.
         */
        void createLogicalDevice();

        /**
         * @brief Crée le pool de commandes Vulkan.
         *
         * Cette fonction crée un pool de commandes Vulkan, qui est utilisé pour allouer des tampons de commandes pour le périphérique logique.
         */
        void createCommandPool();


        // helper functions
        /**
         * @brief Vérifie si le périphérique physique Vulkan spécifié convient aux besoins de l'application.
         *
         * Cette fonction examine les propriétés et les capacités du périphérique pour déterminer s'il convient à l'application.
         *
         * @param device Le périphérique physique Vulkan à évaluer.
         * @return true si le périphérique convient, sinon false.
         */
        bool isDeviceSuitable(VkPhysicalDevice device);

        /**
         * @brief Récupère les extensions Vulkan requises par l'application.
         *
         * Cette fonction retourne un vecteur contenant les noms des extensions Vulkan nécessaires à l'application.
         *
         * @return Un vecteur de pointeurs de chaînes C représentant les noms des extensions requises.
         */
        std::vector<const char*> getRequiredExtensions();

        /**
         * @brief Vérifie la prise en charge des couches de validation Vulkan.
         *
         * Cette fonction vérifie si les couches de validation Vulkan spécifiées sont prises en charge par l'instance Vulkan.
         *
         * @return true si les couches de validation sont prises en charge, sinon false.
         */
        bool checkValidationLayerSupport();

        /**
         * @brief Recherche les familles de files de commandes supportées par le périphérique physique Vulkan spécifié.
         *
         * Cette fonction identifie les indices des familles de files de commandes graphiques et de présentation supportées par le périphérique.
         *
         * @param device Le périphérique physique Vulkan à évaluer.
         * @return Une structure contenant les indices des familles de files de commandes supportées.
         */
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        /**
         * @brief Remplit une structure de données pour la création d'un gestionnaire de débogage Vulkan.
         *
         * Cette fonction initialise une structure de données pour la création d'un gestionnaire de débogage Vulkan avec les informations nécessaires.
         *
         * @param createInfo La structure de données à remplir pour la création du gestionnaire de débogage.
         */
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        /**
         * @brief Vérifie si l'instance Vulkan possède les extensions requises par GLFW.
         *
         * Cette fonction vérifie si l'instance Vulkan possède les extensions requises par GLFW pour créer une surface Vulkan.
         */
        void hasGflwRequiredInstanceExtensions();

        /**
         * @brief Vérifie si le périphérique physique Vulkan spécifié prend en charge les extensions de périphérique nécessaires.
         *
         * Cette fonction vérifie si le périphérique physique Vulkan spécifié prend en charge les extensions de périphérique requises par l'application.
         *
         * @param device Le périphérique physique Vulkan à évaluer.
         * @return true si les extensions de périphérique sont prises en charge, sinon false.
         */
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        /**
         * @brief Interroge le périphérique physique Vulkan spécifié pour obtenir les détails de support de la chaîne d'échange.
         *
         * Cette fonction interroge le périphérique physique Vulkan spécifié pour obtenir les détails de support de la chaîne d'échange, tels que les capacités, les formats de surface supportés et les modes de présentation supportés.
         *
         * @param device Le périphérique physique Vulkan à interroger.
         * @return Une structure contenant les détails de support de la chaîne d'échange.
         */
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);


        VkInstance instance; /**< L'instance Vulkan utilisée par l'application. */

        VkDebugUtilsMessengerEXT debugMessenger; /**< Le gestionnaire de débogage Vulkan pour la gestion des messages de validation et de débogage. */

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; /**< Le périphérique physique Vulkan utilisé par l'application, initialisé à VK_NULL_HANDLE par défaut. */

        LveWindow& window; /**< Une référence à la fenêtre Vulkan utilisée par l'application. */

        VkCommandPool commandPool; /**< Le pool de commandes Vulkan utilisé pour allouer les tampons de commandes. */

        VkDevice device_; /**< Le périphérique logique Vulkan utilisé par l'application. */

        VkSurfaceKHR surface_; /**< La surface Vulkan associée à la fenêtre Vulkan utilisée par l'application. */

        VkQueue graphicsQueue_; /**< La file de commandes pour les opérations graphiques sur le périphérique logique. */

        VkQueue presentQueue_; /**< La file de commandes pour les opérations de présentation sur le périphérique logique. */

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; /**< Les couches de validation Vulkan activées par défaut. */

        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; /**< Les extensions de périphérique Vulkan utilisées par l'application, avec l'extension de la chaîne d'échange Vulkan activée par défaut. */

    };

}  // namespace lve