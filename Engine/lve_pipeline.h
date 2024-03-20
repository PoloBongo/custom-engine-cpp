#pragma once

#include <string>
#include <vector>

namespace lve {
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
		LvePipeline(const std::string& _vertFilepath, const std::string& _fragFilepath);

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
		void CreateGraphicsPipeline(const std::string& _vertFilepath, const std::string& _fragFilepath);
	};

} //namespace lve

