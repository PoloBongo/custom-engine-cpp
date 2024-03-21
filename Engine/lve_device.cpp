#include "lve_device.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace lve {

	// local callback functions
	/**
	 * @brief Fonction de rappel de débogage pour la gestion des messages de validation Vulkan.
	 *
	 * Cette fonction est appelée par les couches de validation Vulkan pour gérer les messages de débogage.
	 * Elle affiche les messages de débogage dans la console d'erreur standard.
	 *
	 * @param messageSeverity La sévérité du message de débogage.
	 * @param messageType Le type de message de débogage.
	 * @param pCallbackData Les données de rappel du messager de débogage.
	 * @param pUserData Des données utilisateur optionnelles.
	 * @return VK_FALSE indiquant que le traitement du message est terminé, et aucune action supplémentaire n'est nécessaire.
	 */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, // Sévérité du message de débogage
		VkDebugUtilsMessageTypeFlagsEXT messageType, // Type de message de débogage
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, // Données de rappel du messager de débogage
		void* pUserData) { // Données utilisateur optionnelles

		// Affiche le message de débogage dans la console d'erreur standard
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		// Indique que le traitement du message est terminé, et qu'aucune action supplémentaire n'est nécessaire
		return VK_FALSE;
	}


	/**
	 * @brief Crée un gestionnaire de messager de débogage Vulkan en utilisant l'extension VK_EXT_debug_utils.
	 *
	 * Cette fonction crée un gestionnaire de messager de débogage Vulkan en utilisant l'extension VK_EXT_debug_utils.
	 * Elle utilise la fonction vkCreateDebugUtilsMessengerEXT, si elle est disponible.
	 *
	 * @param instance L'instance Vulkan.
	 * @param pCreateInfo Les informations de création du gestionnaire de messager de débogage.
	 * @param pAllocator L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire.
	 * @param pDebugMessenger Un pointeur vers l'objet de gestionnaire de messager de débogage à créer.
	 * @return VK_SUCCESS si le gestionnaire de messager de débogage a été créé avec succès, VK_ERROR_EXTENSION_NOT_PRESENT si l'extension n'est pas prise en charge.
	 */
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance, // L'instance Vulkan
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, // Les informations de création du gestionnaire de messager de débogage
		const VkAllocationCallbacks* pAllocator, // L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire
		VkDebugUtilsMessengerEXT* pDebugMessenger) { // Un pointeur vers l'objet de gestionnaire de messager de débogage à créer

		// Récupère le pointeur de fonction pour vkCreateDebugUtilsMessengerEXT
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkCreateDebugUtilsMessengerEXT");

		// Vérifie si la fonction est disponible
		if (func != nullptr) {
			// Appelle la fonction vkCreateDebugUtilsMessengerEXT pour créer le gestionnaire de messager de débogage
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			// L'extension VK_EXT_debug_utils n'est pas prise en charge
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	/**
	 * @brief Détruit un gestionnaire de messager de débogage Vulkan créé avec l'extension VK_EXT_debug_utils.
	 *
	 * Cette fonction détruit un gestionnaire de messager de débogage Vulkan créé avec l'extension VK_EXT_debug_utils.
	 * Elle utilise la fonction vkDestroyDebugUtilsMessengerEXT, si elle est disponible.
	 *
	 * @param instance L'instance Vulkan.
	 * @param debugMessenger L'objet de gestionnaire de messager de débogage à détruire.
	 * @param pAllocator L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire.
	 */
	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance, // L'instance Vulkan
		VkDebugUtilsMessengerEXT debugMessenger, // L'objet de gestionnaire de messager de débogage à détruire
		const VkAllocationCallbacks* pAllocator) { // L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire

		// Récupère le pointeur de fonction pour vkDestroyDebugUtilsMessengerEXT
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkDestroyDebugUtilsMessengerEXT");

		// Vérifie si la fonction est disponible
		if (func != nullptr) {
			// Appelle la fonction vkDestroyDebugUtilsMessengerEXT pour détruire le gestionnaire de messager de débogage
			func(instance, debugMessenger, pAllocator);
		}
	}


	// class member functions

	// Constructeur de la classe LveDevice
	LveDevice::LveDevice(LveWindow& window) : window{ window } {
		createInstance(); // Crée une instance Vulkan
		setupDebugMessenger(); // Configure le messager de débogage Vulkan
		createSurface(); // Crée la surface de la fenêtre Vulkan
		pickPhysicalDevice(); // Sélectionne le périphérique physique adapté
		createLogicalDevice(); // Crée le périphérique logique Vulkan
		createCommandPool(); // Crée le pool de commandes Vulkan
	}

	// Destructeur de la classe LveDevice
	LveDevice::~LveDevice() {
		// Détruit le pool de commandes Vulkan
		vkDestroyCommandPool(device_, commandPool, nullptr);

		// Détruit le périphérique logique Vulkan
		vkDestroyDevice(device_, nullptr);

		// Si les couches de validation sont activées, détruit le messager de débogage Vulkan
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		// Détruit la surface Vulkan associée à la fenêtre
		vkDestroySurfaceKHR(instance, surface_, nullptr);

		// Détruit l'instance Vulkan
		vkDestroyInstance(instance, nullptr);
	}

	/**
	 * @brief Crée une instance Vulkan pour l'application.
	 *
	 * Cette fonction configure et crée une instance Vulkan, qui est la première étape dans l'utilisation de l'API Vulkan.
	 * Elle configure les informations d'application, les extensions requises, les couches de validation, et crée l'instance Vulkan correspondante.
	 *
	 * @throws std::runtime_error si la création de l'instance échoue ou si les couches de validation sont activées mais non disponibles.
	 */
	void LveDevice::createInstance() {
		// Vérifie si les couches de validation sont activées et si elles sont supportées
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// Configuration des informations d'application pour l'instance Vulkan
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "LittleVulkanEngine App";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Configuration de la création de l'instance Vulkan
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Obtention des extensions Vulkan requises pour l'application
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// Configuration du messager de débogage Vulkan si les couches de validation sont activées
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// Création de l'instance Vulkan
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

		// Vérification des extensions GLFW requises pour l'instance Vulkan
		hasGflwRequiredInstanceExtensions();
	}

	/**
	 * @brief Sélectionne le périphérique physique adapté pour l'application.
	 *
	 * Cette fonction récupère la liste des périphériques physiques Vulkan disponibles sur le système,
	 * puis elle itère à travers chaque périphérique pour trouver celui qui convient le mieux à l'application.
	 * Le périphérique sélectionné est stocké dans la variable physicalDevice.
	 *
	 * @throws Une exception si aucun périphérique physique approprié n'est trouvé.
	 */
	void LveDevice::pickPhysicalDevice() {
		// Récupère le nombre de périphériques physiques Vulkan disponibles sur le système
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		// Affiche le nombre de périphériques Vulkan trouvés
		std::cout << "Device count: " << deviceCount << std::endl;

		// Récupère la liste des périphériques physiques Vulkan disponibles sur le système
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Parcourt chaque périphérique pour trouver celui qui convient le mieux à l'application
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		// Vérifie si un périphérique adapté a été trouvé
		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		// Récupère les propriétés du périphérique sélectionné et les affiche
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		std::cout << "physical device: " << properties.deviceName << std::endl;
	}


	/**
	* @brief Crée un périphérique logique Vulkan.
	*
	* Cette fonction crée un périphérique logique Vulkan à partir du périphérique physique sélectionné.
	* Elle configure les files de commandes pour les opérations graphiques et de présentation, ainsi que les fonctionnalités du périphérique.
	*
	* @throws Une exception si la création du périphérique logique échoue.
	*/
	void LveDevice::createLogicalDevice() {
		// Recherche des familles de files de commandes pour le périphérique physique sélectionné
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		// Configuration des files de commandes pour les opérations graphiques et de présentation
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Configuration des fonctionnalités du périphérique
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		// Configuration de la création du périphérique logique
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Configuration des couches de validation si elles sont activées
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Création du périphérique logique Vulkan
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		// Obtention des files de commandes graphiques et de présentation du périphérique logique
		vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
		vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
	}

	/**
	 * @brief Crée un pool de commandes Vulkan.
	 *
	 * Cette fonction crée un pool de commandes Vulkan pour le périphérique logique actuel.
	 * Le pool de commandes est associé à la famille de files de commandes graphiques trouvée pour ce périphérique.
	 * Les tampons de commandes créés à partir de ce pool auront les drapeaux VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	 * et VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, ce qui permet des opérations transitoires et de réinitialiser
	 * les tampons de commandes.
	 *
	 * @throw std::runtime_error si la création du pool de commandes échoue.
	 */
	void LveDevice::createCommandPool() {
		// Recherche les indices des familles de files de commandes supportées par le périphérique physique actuel.
		QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

		// Initialise la structure d'informations du pool de commandes Vulkan.
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; // Indique le type de la structure.
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Spécifie la famille de files de commandes graphiques associée au pool.
		poolInfo.flags =
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Définit les drapeaux du pool de commandes, permettant des opérations transitoires et la réinitialisation des tampons de commandes.

		// Crée le pool de commandes Vulkan en utilisant les informations fournies.
		if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			// Lance une exception si la création du pool de commandes échoue.
			throw std::runtime_error("failed to create command pool!");
		}
	}

	/**
	 * @brief Crée une surface Vulkan associée à la fenêtre de l'application.
	 *
	 * Cette fonction utilise la fenêtre de l'application pour créer une surface Vulkan, qui est nécessaire
	 * pour interagir avec la fenêtre et effectuer le rendu graphique.
	 *
	 * @throw std::runtime_error si la création de la surface échoue.
	 */
	void LveDevice::createSurface() { window.CreateWindowSurface(instance, &surface_); }

	/**
	 * @brief Vérifie si le périphérique physique Vulkan spécifié convient aux besoins de l'application.
	 *
	 * Cette fonction examine les propriétés et les capacités du périphérique pour déterminer s'il convient à l'application.
	 *
	 * @param device Le périphérique physique Vulkan à évaluer.
	 * @return true si le périphérique convient, sinon false.
	 */
	bool LveDevice::isDeviceSuitable(VkPhysicalDevice device) {
		// Recherche des indices des familles de files de commandes supportées par le périphérique physique.
		QueueFamilyIndices indices = findQueueFamilies(device);

		// Vérification de la prise en charge des extensions de périphérique nécessaires.
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		// Vérification de l'adéquation de la chaîne d'échange.
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			// Interrogation du périphérique pour obtenir les détails de support de la chaîne d'échange.
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		// Récupération des fonctionnalités supportées par le périphérique.
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		// Vérification des fonctionnalités requises.
		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
			supportedFeatures.samplerAnisotropy;
	}

	/**
	 * @brief Remplit une structure de données pour la création d'un gestionnaire de débogage Vulkan.
	 *
	 * Cette fonction initialise une structure de données pour la création d'un gestionnaire de débogage Vulkan avec les informations nécessaires.
	 *
	 * @param createInfo La structure de données à remplir pour la création du gestionnaire de débogage.
	 */
	void LveDevice::populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		// Initialise la structure avec des valeurs par défaut.
		createInfo = {};

		// Spécifie le type de la structure.
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

		// Spécifie les niveaux de sévérité des messages à intercepter.
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		// Spécifie les types de messages à intercepter.
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		// Spécifie la fonction de rappel à appeler pour chaque message de débogage.
		createInfo.pfnUserCallback = debugCallback;

		// Spécifie des données utilisateur facultatives.
		createInfo.pUserData = nullptr;  // Optionnel
	}


	/**
	 * @brief Configure le gestionnaire de débogage Vulkan.
	 *
	 * Cette fonction configure le gestionnaire de débogage Vulkan si les couches de validation sont activées.
	 *
	 * Elle utilise la fonction populateDebugMessengerCreateInfo() pour remplir les informations nécessaires pour
	 * la création du gestionnaire de débogage.
	 *
	 * @throw std::runtime_error si la configuration du gestionnaire de débogage échoue.
	 */
	void LveDevice::setupDebugMessenger() {
		// Vérifie si les couches de validation sont activées.
		if (!enableValidationLayers) return;

		// Initialise une structure pour la création du gestionnaire de débogage.
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		// Crée le gestionnaire de débogage avec les informations fournies.
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}


	/**
	 * @brief Vérifie la prise en charge des couches de validation Vulkan.
	 *
	 * Cette fonction vérifie si toutes les couches de validation spécifiées dans la variable `validationLayers`
	 * sont prises en charge par Vulkan sur le système.
	 *
	 * @return true si toutes les couches de validation sont prises en charge, sinon false.
	 */
	bool LveDevice::checkValidationLayerSupport() {
		// Récupère le nombre de couches de validation disponibles.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Récupère les propriétés de toutes les couches de validation disponibles.
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Parcourt toutes les couches de validation requises.
		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			// Vérifie si la couche de validation requise est présente parmi les couches disponibles.
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			// Si la couche de validation requise n'est pas trouvée, retourne false.
			if (!layerFound) {
				return false;
			}
		}

		// Si toutes les couches de validation requises sont trouvées, retourne true.
		return true;
	}


	/**
	 * @brief Récupère les extensions Vulkan requises pour l'instance Vulkan.
	 *
	 * Cette fonction récupère les extensions Vulkan requises pour l'instance Vulkan en utilisant la bibliothèque GLFW.
	 * Elle inclut les extensions requises par GLFW et, si les couches de validation sont activées, l'extension de
	 * gestionnaire de débogage Vulkan.
	 *
	 * @return Un vecteur de pointeurs vers les noms des extensions Vulkan requises.
	 */
	std::vector<const char*> LveDevice::getRequiredExtensions() {
		// Récupère le nombre d'extensions requises par GLFW.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Convertit les noms des extensions GLFW en un vecteur de pointeurs vers les noms des extensions Vulkan.
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// Ajoute l'extension de gestionnaire de débogage Vulkan si les couches de validation sont activées.
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		// Retourne le vecteur contenant les noms des extensions Vulkan requises.
		return extensions;
	}


	/**
	  * @brief Vérifie la disponibilité des extensions Vulkan requises par GLFW.
	  *
	  * Cette fonction vérifie si toutes les extensions Vulkan requises par GLFW sont disponibles sur le système.
	  * Elle récupère d'abord toutes les extensions Vulkan disponibles, puis compare avec les extensions requises
	  * obtenues à partir de la fonction getRequiredExtensions(). Si une extension requise est manquante, une exception
	  * est levée.
	  *
	  * @throw std::runtime_error si une extension requise par GLFW est manquante.
	  */
	void LveDevice::hasGflwRequiredInstanceExtensions() {
		// Récupère le nombre d'extensions Vulkan disponibles.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Affiche les extensions Vulkan disponibles et les stocke dans un ensemble pour une recherche rapide.
		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions) {
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		// Affiche les extensions Vulkan requises par GLFW et vérifie leur disponibilité.
		std::cout << "required extensions:" << std::endl;
		auto requiredExtensions = getRequiredExtensions();
		for (const auto& required : requiredExtensions) {
			std::cout << "\t" << required << std::endl;
			if (available.find(required) == available.end()) {
				// Lance une exception si une extension requise est manquante.
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}


	/**
	 * @brief Vérifie la prise en charge des extensions de périphérique Vulkan par le périphérique physique spécifié.
	 *
	 * Cette fonction vérifie si toutes les extensions de périphérique Vulkan requises par l'application sont prises en charge
	 * par le périphérique physique Vulkan spécifié.
	 *
	 * @param device Le périphérique physique Vulkan à vérifier.
	 * @return true si toutes les extensions de périphérique requises sont prises en charge, sinon false.
	 */
	bool LveDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		// Récupère le nombre d'extensions de périphérique Vulkan disponibles pour le périphérique spécifié.
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// Récupère les propriétés de toutes les extensions de périphérique Vulkan disponibles.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			device,
			nullptr,
			&extensionCount,
			availableExtensions.data());

		// Crée un ensemble contenant toutes les extensions de périphérique requises par l'application.
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Parcourt toutes les extensions de périphérique disponibles.
		for (const auto& extension : availableExtensions) {
			// Supprime les extensions disponibles de l'ensemble des extensions requises.
			requiredExtensions.erase(extension.extensionName);
		}

		// Si l'ensemble des extensions requises est vide, cela signifie que toutes les extensions requises sont prises en charge.
		return requiredExtensions.empty();
	}


	/**
	 * @brief Recherche et retourne les indices des files d'attente de périphériques pour le périphérique physique spécifié.
	 *
	 * Cette fonction recherche les indices des files d'attente de périphériques pour les opérations de rendu graphique et de présentation
	 * pour le périphérique physique Vulkan spécifié.
	 *
	 * @param device Le périphérique physique Vulkan pour lequel les files d'attente doivent être recherchées.
	 * @return Une structure QueueFamilyIndices contenant les indices des files d'attente pour les opérations de rendu graphique et de présentation.
	 */
	QueueFamilyIndices LveDevice::findQueueFamilies(VkPhysicalDevice device) {
		// Initialise la structure QueueFamilyIndices pour stocker les indices des files d'attente.
		QueueFamilyIndices indices;

		// Récupère le nombre de familles de files d'attente de périphériques disponibles pour le périphérique spécifié.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// Récupère les propriétés de toutes les familles de files d'attente de périphériques.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		// Parcourt toutes les familles de files d'attente pour trouver celles qui prennent en charge les opérations de rendu graphique et de présentation.
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			// Vérifie si la famille de files d'attente prend en charge les opérations de rendu graphique.
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}
			// Vérifie si la famille de files d'attente prend en charge la présentation sur la surface associée.
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}
			// Si les indices requis ont été trouvés, arrête la recherche.
			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		// Retourne les indices des files d'attente trouvées.
		return indices;
	}


	/**
	 * @brief Interroge et retourne les détails de prise en charge de la chaîne d'échange de swap (SwapChain) pour le périphérique physique spécifié.
	 *
	 * Cette fonction interroge le périphérique physique Vulkan spécifié pour obtenir les détails de prise en charge de la chaîne d'échange de swap, tels que
	 * les capacités de surface, les formats de surface pris en charge et les modes de présentation pris en charge.
	 *
	 * @param device Le périphérique physique Vulkan pour lequel les détails de prise en charge de la chaîne d'échange de swap doivent être interrogés.
	 * @return Une structure SwapChainSupportDetails contenant les détails de prise en charge de la chaîne d'échange de swap.
	 */
	SwapChainSupportDetails LveDevice::querySwapChainSupport(VkPhysicalDevice device) {
		// Initialise la structure SwapChainSupportDetails pour stocker les détails de prise en charge de la chaîne d'échange de swap.
		SwapChainSupportDetails details;

		// Interroge le périphérique physique Vulkan pour obtenir les capacités de la surface associée.
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		// Récupère le nombre de formats de surface pris en charge par le périphérique.
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		// Si des formats sont pris en charge, récupère les détails de ces formats.
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		}

		// Récupère le nombre de modes de présentation pris en charge par le périphérique.
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		// Si des modes de présentation sont pris en charge, récupère les détails de ces modes.
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				surface_,
				&presentModeCount,
				details.presentModes.data());
		}

		// Retourne les détails de prise en charge de la chaîne d'échange de swap.
		return details;
	}


	/**
	 * @brief Recherche un format d'image supporté parmi une liste de formats candidats.
	 *
	 * Cette fonction recherche un format d'image supporté parmi une liste de formats candidats,
	 * en tenant compte du mode de tiling spécifié (linéaire ou optimal) et des fonctionnalités requises.
	 *
	 * @param candidates La liste des formats candidats à vérifier.
	 * @param tiling Le mode de tiling de l'image (VK_IMAGE_TILING_LINEAR ou VK_IMAGE_TILING_OPTIMAL).
	 * @param features Les fonctionnalités requises pour le format.
	 * @return Le format d'image supporté trouvé.
	 * @throws std::runtime_error si aucun format d'image supporté n'est trouvé.
	 */
	VkFormat LveDevice::findSupportedFormat(
		const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		// Parcourt chaque format d'image candidat
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			// Obtient les propriétés du format
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			// Vérifie si le format est adapté au mode de tiling spécifié (linéaire ou optimal)
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				// Si le format est adapté au mode linéaire et prend en charge toutes les fonctionnalités requises, le retourne
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				// Si le format est adapté au mode optimal et prend en charge toutes les fonctionnalités requises, le retourne
				return format;
			}
		}
		// Si aucun format d'image trouvé ne répond aux critères spécifiés, lance une exception
		throw std::runtime_error("failed to find supported format!");
	}


	/**
	 * @brief Recherche un type de mémoire adapté pour les allocations de mémoire graphique.
	 *
	 * Cette fonction recherche un type de mémoire adapté pour les allocations de mémoire graphique,
	 * en tenant compte du filtre de type spécifié et des propriétés de mémoire requises.
	 *
	 * @param typeFilter Le filtre de type de mémoire spécifié.
	 * @param properties Les propriétés de mémoire requises.
	 * @return L'index du type de mémoire adapté trouvé.
	 * @throws std::runtime_error si aucun type de mémoire adapté n'est trouvé.
	 */
	uint32_t LveDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		// Parcourt chaque type de mémoire
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			// Vérifie si le type de mémoire est compatible avec le filtre spécifié et possède toutes les propriétés requises
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				// Si le type de mémoire convient, retourne son index
				return i;
			}
		}
		// Si aucun type de mémoire adapté n'est trouvé, lance une exception
		throw std::runtime_error("failed to find suitable memory type!");
	}


	/**
	 * @brief Crée un tampon Vulkan avec la taille, l'utilisation et les propriétés spécifiées.
	 *
	 * Cette fonction crée un tampon Vulkan avec la taille, l'utilisation et les propriétés spécifiées,
	 * et associe la mémoire appropriée au tampon.
	 *
	 * @param size La taille du tampon en octets.
	 * @param usage Les indicateurs d'utilisation du tampon.
	 * @param properties Les propriétés de mémoire du tampon.
	 * @param buffer Une référence à l'objet tampon à créer.
	 * @param bufferMemory Une référence à la mémoire allouée pour le tampon.
	 * @throws std::runtime_error en cas d'échec de la création ou de l'allocation de mémoire pour le tampon.
	 */
	void LveDevice::createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory) {
		// Définit les informations du tampon
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// Crée le tampon
		if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		// Obtient les exigences de mémoire pour le tampon
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

		// Alloue la mémoire pour le tampon
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			// En cas d'échec de l'allocation de mémoire, détruit le tampon créé
			vkDestroyBuffer(device_, buffer, nullptr);
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		// Associe la mémoire allouée au tampon
		vkBindBufferMemory(device_, buffer, bufferMemory, 0);
	}


	/**
	 * @brief Démarre une série de commandes Vulkan pour une utilisation unique.
	 *
	 * Cette fonction alloue et démarre un tampon de commandes Vulkan pour une utilisation unique.
	 * Le tampon de commandes retourné peut être utilisé pour exécuter des commandes Vulkan
	 * qui doivent être exécutées une seule fois.
	 *
	 * @return Le tampon de commandes Vulkan alloué et démarré.
	 * @throws std::runtime_error en cas d'échec de l'allocation du tampon de commandes.
	 */
	VkCommandBuffer LveDevice::beginSingleTimeCommands() {
		// Alloue un tampon de commandes
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		if (vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffer!");
		}

		// Démarre l'enregistrement des commandes dans le tampon
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			// En cas d'échec du démarrage de l'enregistrement des commandes, libère le tampon de commandes alloué
			vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}


	/**
	 * @brief Termine une série de commandes Vulkan pour une utilisation unique.
	 *
	 * Cette fonction termine l'enregistrement des commandes dans le tampon de commandes spécifié,
	 * soumet les commandes à la file de commandes graphiques pour exécution, attend la fin de l'exécution
	 * des commandes, puis libère le tampon de commandes.
	 *
	 * @param commandBuffer Le tampon de commandes Vulkan à terminer et à soumettre.
	 * @throws std::runtime_error en cas d'échec de soumission des commandes ou d'attente de la fin de l'exécution.
	 */
	void LveDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		// Termine l'enregistrement des commandes dans le tampon de commandes
		vkEndCommandBuffer(commandBuffer);

		// Soumet les commandes à la file de commandes graphiques pour exécution
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit command buffer to graphics queue!");
		}

		// Attend la fin de l'exécution des commandes
		if (vkQueueWaitIdle(graphicsQueue_) != VK_SUCCESS) {
			throw std::runtime_error("failed to wait for graphics queue to idle!");
		}

		// Libère le tampon de commandes
		vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
	}


	/**
	 * @brief Copie les données d'un tampon source vers un tampon de destination.
	 *
	 * Cette fonction utilise un tampon de commandes temporaire pour copier les données
	 * du tampon source vers le tampon de destination. Elle garantit que l'opération de
	 * copie se produit de manière synchrone, c'est-à-dire que le tampon de commandes temporaire
	 * est soumis à la file de commandes graphiques, attend que toutes les opérations en cours
	 * sur cette file soient terminées, puis est libéré.
	 *
	 * @param srcBuffer Le tampon source à partir duquel copier les données.
	 * @param dstBuffer Le tampon de destination vers lequel copier les données.
	 * @param size La taille des données à copier, en octets.
	 * @throws std::runtime_error en cas d'échec de l'allocation ou de l'enregistrement des commandes.
	 */
	void LveDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		// Définit la région de copie
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Facultatif
		copyRegion.dstOffset = 0;  // Facultatif
		copyRegion.size = size;

		// Effectue la copie des données entre les tampons
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		// Termine l'enregistrement des commandes et soumet le tampon de commandes à la file de commandes graphiques
		endSingleTimeCommands(commandBuffer);
	}


	/**
	 * @brief Copie les données d'un tampon vers une image Vulkan.
	 *
	 * Cette fonction utilise un tampon de commandes temporaire pour copier les données
	 * du tampon vers l'image Vulkan. Elle garantit que l'opération de copie se produit de manière
	 * synchrone, c'est-à-dire que le tampon de commandes temporaire est soumis à la file de commandes
	 * graphiques, attend que toutes les opérations en cours sur cette file soient terminées, puis est libéré.
	 *
	 * @param buffer Le tampon contenant les données à copier vers l'image.
	 * @param image L'image Vulkan de destination vers laquelle copier les données.
	 * @param width La largeur de l'image en pixels.
	 * @param height La hauteur de l'image en pixels.
	 * @param layerCount Le nombre de couches de l'image.
	 * @throws std::runtime_error en cas d'échec de l'allocation ou de l'enregistrement des commandes.
	 */
	void LveDevice::copyBufferToImage(
		VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		// Configure la région de copie
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		// Copie les données du tampon vers l'image Vulkan
		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		// Termine l'enregistrement des commandes et soumet le tampon de commandes à la file de commandes graphiques
		endSingleTimeCommands(commandBuffer);
	}


	/**
	 * @brief Crée une image Vulkan avec les informations spécifiées.
	 *
	 * Cette fonction crée une image Vulkan en utilisant les informations fournies dans
	 * l'objet VkImageCreateInfo. Elle alloue également la mémoire nécessaire pour cette
	 * image et associe cette mémoire à l'image créée.
	 *
	 * @param imageInfo Les informations nécessaires à la création de l'image.
	 * @param properties Les propriétés de la mémoire de l'image.
	 * @param image Référence où l'objet image créé sera stocké.
	 * @param imageMemory Référence où la mémoire de l'image sera stockée.
	 * @throws std::runtime_error en cas d'échec de la création de l'image ou de l'allocation de mémoire.
	 */
	void LveDevice::createImageWithInfo(
		const VkImageCreateInfo& imageInfo,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory) {
		// Crée l'image Vulkan
		if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		// Récupère les exigences de mémoire de l'image
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_, image, &memRequirements);

		// Alloue la mémoire pour l'image
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			// Libère l'image si l'allocation de mémoire échoue
			vkDestroyImage(device_, image, nullptr);
			throw std::runtime_error("failed to allocate image memory!");
		}

		// Associe la mémoire allouée à l'image
		if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) {
			// Libère l'image et la mémoire si la liaison échoue
			vkDestroyImage(device_, image, nullptr);
			vkFreeMemory(device_, imageMemory, nullptr);
			throw std::runtime_error("failed to bind image memory!");
		}
	}

}  // namespace lve