#pragma once

#include "lve_device.h"

#include <string>
#include <vector>

namespace lve {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};



	class LvePipeline
	{
	public:

		/**
		 * @brief Constructeur de la classe LvePipeline.
		 *
		 * Initialise un pipeline graphique en créant les shaders à partir des fichiers spécifiés.
		 *
		 * @param _vertFilepath Le chemin d'accès au fichier contenant le code source du shader de vertex.
		 * @param _fragFilepath Le chemin d'accès au fichier contenant le code source du shader de fragment.
		 */
		LvePipeline(LveDevice & _device, const std::string& _vertFilepath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo);

		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete;
		LvePipeline &operator=(const LvePipeline&) = delete;

		/**
		 * @brief Lie le pipeline graphique au tampon de commandes spécifié.
		 *
		 * Cette méthode lie le pipeline graphique associé à ce LvePipeline à un tampon de commandes spécifié.
		 *
		 * @param _commandBuffer Le tampon de commandes auquel le pipeline graphique doit être lié.
		 */
		void Bind(VkCommandBuffer commandBuffer);

		/**
		 * @brief Configure les paramètres par défaut du pipeline.
		 *
		 * Cette méthode configure les paramètres par défaut du pipeline en remplissant la structure PipelineConfigInfo fournie.
		 *
		 * @param _configInfo Une référence vers la structure PipelineConfigInfo à remplir avec les paramètres par défaut du pipeline.
		 */
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo);
		static void EnableAlphaBlending(PipelineConfigInfo& _configInfo);

	private:

		/**
		* @brief Lit le contenu d'un fichier binaire et le retourne sous forme de vecteur de caractères.
		 *
		 * Cette fonction ouvre un fichier binaire, lit son contenu dans un vecteur de caractères et le retourne.
		 *
		* @param _filepath Le chemin d'accès au fichier à lire.
		* @return Un vecteur de caractères contenant le contenu du fichier.
		* @throws std::runtime_error si le fichier ne peut pas être ouvert.
		*/
		static std::vector<char> ReadFile(const std::string& _filepath);

		/**
		* @brief Crée un pipeline graphique en chargeant les shaders depuis les fichiers spécifiés.
		*
		* Cette fonction charge le code source des shaders de vertex et de fragment depuis les fichiers spécifiés,
		* puis affiche la taille du code source des shaders.
		*
		* @param _vertFilepath Le chemin d'accès au fichier contenant le code source du shader de vertex.
		* @param _fragFilepath Le chemin d'accès au fichier contenant le code source du shader de fragment.
		*/
		void CreateGraphicsPipeline(const std::string& _vertFilepath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo);

		/**
		 * @brief Crée un module de shader à partir du code du shader spécifié.
		 *
		 * Cette méthode crée un module de shader Vulkan à partir du code du shader spécifié.
		 *
		 * @param _code Le code du shader sous forme d'un vecteur de caractères.
		 * @param _shaderModule Un pointeur vers l'emplacement où stocker le module de shader créé.
		 *
		 * @throw std::runtime_error si la création du module de shader échoue.
		 */
		void CreateShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule);

		LveDevice& lveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

} //namespace lve

