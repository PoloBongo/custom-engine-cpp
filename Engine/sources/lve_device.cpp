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
		vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		vk::DebugUtilsMessageTypeFlagsEXT messageType,
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

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
	vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger)
    {
        PFN_vkVoidFunction function = instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
        PFN_vkGetInstanceProcAddr messenger_ext = reinterpret_cast<PFN_vkGetInstanceProcAddr>(function);
        vk::DispatchLoaderDynamic dispatch(instance, messenger_ext);
        vk::Result result = instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, pDebugMessenger, dispatch);
        return result;
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
		vk::Instance instance,
		vk::DebugUtilsMessengerEXT debugMessenger,
		const vk::AllocationCallbacks* pAllocator) {

		// Récupère le pointeur de fonction pour vkDestroyDebugUtilsMessengerEXT
		auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

		// Vérifie si la fonction est disponible
		if (func != nullptr) {
			// Appelle la fonction vkDestroyDebugUtilsMessengerEXT pour détruire le gestionnaire de messager de débogage
			func(static_cast<vk::Instance > (instance), static_cast<VkDebugUtilsMessengerEXT>(debugMessenger), reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
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
		device_.destroyCommandPool(commandPool, nullptr);

		// Détruit le périphérique logique Vulkan
		device_.destroy(nullptr);

		// Si les couches de validation sont activées, détruit le messager de débogage Vulkan
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		// Détruit la surface Vulkan associée à la fenêtre
		instance.destroySurfaceKHR(surface_, nullptr);

		// Détruit l'instance Vulkan
		instance.destroy(nullptr);
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
		vk::ApplicationInfo appInfo(
			"LittleVulkanEngine App",
			VK_MAKE_VERSION(1, 0, 0),
			"No Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_0
		);

		// Obtention des extensions Vulkan requises pour l'application
		auto extensions = getRequiredExtensions();

		// Configuration de la création de l'instance Vulkan
		vk::InstanceCreateInfo createInfo(
			{},
			&appInfo, // Initialisation directe avec l'adresse de appInfo
			0, nullptr, // Aucune couche de validation
			static_cast<uint32_t>(extensions.size()), extensions.data() // Extensions activées
		);

		// Configuration du messager de débogage Vulkan si les couches de validation sont activées
		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// Création de l'instance Vulkan
		instance = vk::createInstance(createInfo);

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
		// Récupère la liste des périphériques physiques Vulkan disponibles sur le système
		std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

		// Vérifie si des périphériques Vulkan ont été trouvés
		if (devices.empty()) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		// Affiche le nombre de périphériques Vulkan trouvés
		std::cout << "Device count: " << devices.size() << std::endl;

		// Parcourt chaque périphérique pour trouver celui qui convient le mieux à l'application
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		// Vérifie si un périphérique adapté a été trouvé
		if (!physicalDevice) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		// Récupère les propriétés du périphérique sélectionné et les affiche
		vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
		std::cout << "Physical device: " << properties.deviceName << std::endl;
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
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo(
				{},
				queueFamily,
				1,
				&queuePriority
			);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Configuration des fonctionnalités du périphérique
		vk::PhysicalDeviceFeatures deviceFeatures;
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		// Configuration de la création du périphérique logique
		vk::DeviceCreateInfo createInfo;
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

		// Création du périphérique logique Vulkan
		device_ = physicalDevice.createDevice(createInfo);

		// Obtention des files de commandes graphiques et de présentation du périphérique logique
		graphicsQueue_ = device_.getQueue(indices.graphicsFamily, 0);
		presentQueue_ = device_.getQueue(indices.presentFamily, 0);
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
		vk::CommandPoolCreateInfo poolInfo(
			vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
			queueFamilyIndices.graphicsFamily
		);

		// Crée le pool de commandes Vulkan en utilisant les informations fournies.
		try {
			commandPool = device_.createCommandPool(poolInfo);
		}
		catch (const vk::SystemError& e) {
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
	bool LveDevice::isDeviceSuitable(vk::PhysicalDevice _device) {
		// Recherche des indices des familles de files de commandes supportées par le périphérique physique.
		QueueFamilyIndices indices = findQueueFamilies(_device);

		// Vérification de la prise en charge des extensions de périphérique nécessaires.
		bool extensionsSupported = checkDeviceExtensionSupport(_device);

		// Vérification de l'adéquation de la chaîne d'échange.
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			// Interrogation du périphérique pour obtenir les détails de support de la chaîne d'échange.
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		// Récupération des fonctionnalités supportées par le périphérique.
		vk::PhysicalDeviceFeatures supportedFeatures = _device.getFeatures();

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
		vk::DebugUtilsMessengerCreateInfoEXT& _createInfo) {
		// Spécifie le type de la structure.
		_createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;

		// Spécifie les niveaux de sévérité des messages à intercepter.
		_createInfo.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

		// Spécifie les types de messages à intercepter.
		_createInfo.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

		// Spécifie la fonction de rappel à appeler pour chaque message de débogage.
		_createInfo.setPfnUserCallback((PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback);

		// Spécifie des données utilisateur facultatives.
		_createInfo.setPUserData(nullptr);  // Optionnel
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
	void LveDevice::setupDebugMessenger()
	{
		// Vérifie si les couches de validation sont activées.
		if (!enableValidationLayers)
			return;

		// Initialise une structure pour la création du gestionnaire de débogage.
		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		// Crée le gestionnaire de débogage avec les informations fournies.
		try
		{
			vk::DispatchLoaderDynamic dispatcher = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
			if (vk::Result result = instance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &debugMessenger, dispatcher); result != vk::Result::eSuccess)
				throw std::runtime_error("failed to set up debug messenger!");
		}
		catch (const vk::SystemError& e)
		{
			throw std::runtime_error("failed to set up debug messenger: " + std::string(e.what()));
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
		// Récupère les propriétés de toutes les couches de validation disponibles.
		auto availableLayers = vk::enumerateInstanceLayerProperties();

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
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

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
		auto availableExtensions = vk::enumerateInstanceExtensionProperties();

		// Affiche les extensions Vulkan disponibles et les stocke dans un ensemble pour une recherche rapide.
		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : availableExtensions) {
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
	bool LveDevice::checkDeviceExtensionSupport(vk::PhysicalDevice _device) {
		// Récupère les propriétés de toutes les extensions de périphérique Vulkan disponibles pour le périphérique spécifié.
		auto availableExtensions = _device.enumerateDeviceExtensionProperties(nullptr);

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
	QueueFamilyIndices LveDevice::findQueueFamilies(vk::PhysicalDevice device) {
		// Initialise la structure QueueFamilyIndices pour stocker les indices des files d'attente.
		QueueFamilyIndices indices;

		// Récupère le nombre de familles de files d'attente de périphériques disponibles pour le périphérique spécifié.
		auto queueFamilies = device.getQueueFamilyProperties();

		// Parcourt toutes les familles de files d'attente pour trouver celles qui prennent en charge les opérations de rendu graphique et de présentation.
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			// Vérifie si la famille de files d'attente prend en charge les opérations de rendu graphique.
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}
			// Vérifie si la famille de files d'attente prend en charge la présentation sur la surface associée.
			bool presentSupport = device.getSurfaceSupportKHR(i, surface_);
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
	SwapChainSupportDetails LveDevice::querySwapChainSupport(vk::PhysicalDevice device) {
		// Initialise la structure SwapChainSupportDetails pour stocker les détails de prise en charge de la chaîne d'échange de swap.
		SwapChainSupportDetails details;

		// Interroge le périphérique physique Vulkan pour obtenir les capacités de la surface associée.
		details.capabilities = device.getSurfaceCapabilitiesKHR(surface_);

		// Récupère les formats de surface pris en charge par le périphérique.
		details.formats = device.getSurfaceFormatsKHR(surface_);

		// Récupère les modes de présentation pris en charge par le périphérique.
		details.presentModes = device.getSurfacePresentModesKHR(surface_);

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
	vk::Format LveDevice::findSupportedFormat(
		const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
		// Parcourt chaque format d'image candidat
		for (vk::Format format : candidates) {
			vk::FormatProperties props;
			// Obtient les propriétés du format
			props = physicalDevice.getFormatProperties(format);

			// Vérifie si le format est adapté au mode de tiling spécifié (linéaire ou optimal)
			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
				// Si le format est adapté au mode linéaire et prend en charge toutes les fonctionnalités requises, le retourne
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
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
	uint32_t LveDevice::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		vk::PhysicalDeviceMemoryProperties memProperties;
		memProperties = physicalDevice.getMemoryProperties();

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
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory) {

		// Définit les informations du tampon
		vk::BufferCreateInfo bufferInfo(
			{},
			size,
			usage,
			vk::SharingMode::eExclusive
		);

		// Crée le tampon
		buffer = device_.createBuffer(bufferInfo);

		// Obtient les exigences de mémoire pour le tampon
		vk::MemoryRequirements memRequirements;
		memRequirements = device_.getBufferMemoryRequirements(buffer);

		// Alloue la mémoire pour le tampon
		vk::MemoryAllocateInfo allocInfo(
			memRequirements.size,
			findMemoryType(memRequirements.memoryTypeBits, properties)
		);

		bufferMemory = device_.allocateMemory(allocInfo);

		// Associe la mémoire allouée au tampon
		device_.bindBufferMemory(buffer, bufferMemory, 0);
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
	vk::CommandBuffer LveDevice::beginSingleTimeCommands() {
		// Alloue un tampon de commandes
		vk::CommandBufferAllocateInfo allocInfo(
			commandPool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

		vk::CommandBuffer commandBuffer = device_.allocateCommandBuffers(allocInfo)[0];

		// Démarre l'enregistrement des commandes dans le tampon
		vk::CommandBufferBeginInfo beginInfo(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		);

		commandBuffer.begin(beginInfo);

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
	void LveDevice::endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
		// Termine l'enregistrement des commandes dans le tampon de commandes
		commandBuffer.end();

		// Soumet les commandes à la file de commandes graphiques pour exécution
		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		graphicsQueue_.submit(submitInfo, nullptr);

		// Attend la fin de l'exécution des commandes
		graphicsQueue_.waitIdle();

		// Libère le tampon de commandes
		device_.freeCommandBuffers(commandPool, 1, &commandBuffer);
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
	void LveDevice::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

		// Définit la région de copie
		vk::BufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Facultatif
		copyRegion.dstOffset = 0;  // Facultatif
		copyRegion.size = size;

		// Effectue la copie des données entre les tampons
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

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
		vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount) {
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

		// Configure la région de copie
		vk::BufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = vk::Offset3D{ 0, 0, 0 };
		region.imageExtent = vk::Extent3D{ width, height, 1 };

		// Copie les données du tampon vers l'image Vulkan
		commandBuffer.copyBufferToImage(
			buffer,
			image,
			vk::ImageLayout::eTransferDstOptimal,
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
		const vk::ImageCreateInfo& imageInfo,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory) {
		// Crée l'image Vulkan
		try {
			image = device_.createImage(imageInfo);
		}
		catch (const vk::SystemError& e) {
			throw std::runtime_error("failed to create image!");
		}

		// Récupère les exigences de mémoire de l'image
		vk::MemoryRequirements memRequirements;
		memRequirements = device_.getImageMemoryRequirements(image);

		// Alloue la mémoire pour l'image
		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.setAllocationSize(memRequirements.size);
		allocInfo.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, properties));

		try {
			imageMemory = device_.allocateMemory(allocInfo);
		}
		catch (const vk::SystemError& e) {
			// Libère l'image si l'allocation de mémoire échoue
			device_.destroyImage(image);
			throw std::runtime_error("failed to allocate image memory!");
		}

		// Associe la mémoire allouée à l'image
		try {
			device_.bindImageMemory(image, imageMemory, 0);
		}
		catch (const vk::SystemError& e) {
			// Libère l'image et la mémoire si la liaison échoue
			device_.destroyImage(image);
			device_.freeMemory(imageMemory);
			throw std::runtime_error("failed to bind image memory!");
		}
	}



}  // namespace lve