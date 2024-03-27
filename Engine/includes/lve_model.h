#pragma once

#include <vulkan/vulkan.hpp>
#include "lve_device.h"
#include "lve_buffer.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

//std
#include <memory>
#include <vector>
#include <string>

namespace lve {
class LveModel
{

public:

	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};


		/**
		 * @brief Obtient les descriptions de liaison des entrées de vertex.
		 *
		 * Cette fonction retourne les descriptions de liaison des entrées de vertex pour le pipeline de rendu.
		 * Elle crée un vecteur de descriptions de liaison avec une seule description, où le binding est défini sur 0,
		 * la stride est définie sur la taille d'une structure Vertex, et le taux d'entrée est défini sur VK_VERTEX_INPUT_RATE_VERTEX.
		 *
		 * @return Un vecteur contenant les descriptions de liaison des entrées de vertex.
		 */
		static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();

		/**
		 * @brief Obtient les descriptions d'attribut des entrées de vertex.
		 *
		 * Cette fonction retourne les descriptions d'attribut des entrées de vertex pour le pipeline de rendu.
		 * Elle crée un vecteur de descriptions d'attribut et y ajoute des descriptions pour chaque attribut de la structure Vertex.
		 * Chaque description contient l'index de liaison, l'emplacement, le format de données et le décalage de l'attribut correspondant dans la structure Vertex.
		 *
		 * @return Un vecteur contenant les descriptions d'attribut des entrées de vertex.
		 */
		static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& _other) const {
			return position == _other.position && color == _other.color && normal == _other.normal && uv == _other.uv;
		}
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		/**
		 * @brief Charge un modèle à partir d'un fichier.
		 *
		 * Cette fonction charge un modèle à partir d'un fichier au format OBJ. Elle utilise la bibliothèque tinyobjloader pour lire le fichier.
		 * Les attributs des vertices tels que la position, la couleur, la normale et les coordonnées de texture sont extraits du fichier et utilisés pour créer les vertices du modèle.
		 * Les vertices sont stockés dans le vecteur 'vertices' de l'objet LveModel::Builder, tandis que les indices des vertices sont stockés dans le vecteur 'indices'.
		 * Les vertices sont comparés pour détecter les doublons et les indices sont ajustés en conséquence pour assurer l'unicité des vertices.
		 *
		 * @param _filepath Le chemin du fichier du modèle.
		 * @throws std::runtime_error Si une erreur survient lors du chargement du modèle.
		 */
		void LoadModel(const std::string& _filepath);
	};

	LveModel(LveDevice& _lveDevice, const LveModel::Builder &_builder);
	~LveModel();

	LveModel(const LveModel&) = delete;
	LveModel operator=(const LveModel&) = delete;


	/**
	 * @brief Crée un modèle à partir d'un fichier.
	 *
	 * Cette fonction statique crée un modèle à partir d'un fichier en utilisant un objet LveDevice et un chemin de fichier donnés.
	 * Elle charge d'abord le modèle à l'aide du constructeur Builder, puis affiche le nombre de vertices chargés.
	 * Enfin, elle crée un modèle unique à partir de l'objet LveDevice et du constructeur Builder, puis le retourne.
	 *
	 * @param _device L'objet LveDevice utilisé pour créer le modèle.
	 * @param _filepath Le chemin du fichier à partir duquel charger le modèle.
	 * @return Un pointeur unique vers le modèle créé.
	 */
	static std::unique_ptr<LveModel> CreateModelFromFile(LveDevice& _device, const std::string& _filepath);

	/**
	 * @brief Lie les tampons de vertex et d'index au tampon de commande spécifié.
	 *
	 * Cette fonction lie les tampons de vertex et d'index au tampon de commande spécifié pour le rendu ultérieur.
	 * Si un tampon d'index est présent, il est également lié au tampon de commande avec le type d'index spécifié.
	 *
	 * @param _commandBuffer Le tampon de commande auquel lier les tampons de vertex et d'index.
	 */
	void Bind(vk::CommandBuffer _commandBuffer);

	/**
	 * @brief Dessine les primitives géométriques à l'aide du tampon de commande spécifié.
	 *
	 * Cette fonction dessine les primitives géométriques à l'aide du tampon de commande spécifié.
	 * Si un tampon d'index est présent, la fonction utilise vkCmdDrawIndexed pour dessiner les primitives indexées. Sinon, elle utilise vkCmdDraw pour dessiner les primitives non indexées.
	 *
	 * @param _commandBuffer Le tampon de commande utilisé pour dessiner les primitives géométriques.
	 */
	void Draw(vk::CommandBuffer _commandBuffer);

private:
	/**
	 * @brief Crée le tampon de vertex.
	 *
	 * Cette fonction crée le tampon de vertex à partir des données de vertex fournies.
	 * Elle calcule la taille du tampon en fonction du nombre de vertices et de leur taille individuelle, puis crée un tampon de transfert de mémoire visible par l'hôte pour copier les données de vertex.
	 * Les données de vertex sont ensuite copiées dans le tampon de transfert, puis transférées vers un tampon de mémoire local du périphérique pour une utilisation efficace pendant le rendu.
	 *
	 * @param _vertices Les données de vertex à utiliser pour créer le tampon.
	 */
	void CreateVertexBuffer(const std::vector<Vertex>& _vertices);


	/**
	 * @brief Crée le tampon d'index.
	 *
	 * Cette fonction crée le tampon d'index à partir des indices fournis.
	 * Elle détermine d'abord si des indices sont présents. Si aucun indice n'est présent, la fonction se termine.
	 * Sinon, elle calcule la taille du tampon en fonction du nombre d'indices et de leur taille individuelle, puis crée un tampon de transfert de mémoire visible par l'hôte pour copier les données d'indices.
	 * Les données d'indices sont ensuite copiées dans le tampon de transfert, puis transférées vers un tampon de mémoire local du périphérique pour une utilisation efficace pendant le rendu.
	 *
	 * @param _indices Les indices à utiliser pour créer le tampon.
	 */
	void CreateIndexBuffer(const std::vector<uint32_t>& _indices);

	LveDevice& lveDevice;

	std::unique_ptr<LveBuffer> vertexBuffer;
	uint32_t vertexCount;

	bool hasIndexBuffer = false;
	std::unique_ptr<LveBuffer> indexBuffer;
	uint32_t indexCount;

};


} //lve
