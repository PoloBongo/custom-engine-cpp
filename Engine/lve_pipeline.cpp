#include "lve_pipeline.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lve {

	LvePipeline::LvePipeline(const std::string& _vertFilepath, const std::string& _fragFilepath) {
		CreateGraphicsPipeline(_vertFilepath, _fragFilepath);
	}

	std::vector<char> LvePipeline::ReadFile(const std::string& _filepath) {
		//std::ios::ate = quand le fichier est ouvert, on cherche la fin directement (obetnir la taille), std::ios::binary = éviter les transformations de text
		std::ifstream file{ _filepath, std::ios::ate | std::ios::binary }; // Ouvre le fichier en mode binaire et cherche la fin pour obtenir la taille

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file : " + _filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg()); //tellg pour obtenir la dernière position donc la taille du fichier
		std::vector<char> buffer(fileSize); // alloue un tampon de la taille du fichier

		file.seekg(0); // Remet la position de lecture au début du fichier
		file.read(buffer.data(), fileSize); // Lit le contenu du fichier dans le tampon

		file.close(); // Ferme le fichier

		return buffer; // Retourne le tampon contenant le contenu du fichier
	}

	void LvePipeline::CreateGraphicsPipeline(const std::string& _vertFilepath, const std::string& _fragFilepath) {
		auto vertCode = ReadFile(_vertFilepath);
		auto fragCode = ReadFile(_fragFilepath);

		std::cout << "Vertex Shader Code Size : " << vertCode.size() << std::endl;
		std::cout << "Fragment Shader Code Size : " << fragCode.size() << std::endl;
	}
} //namespace lve