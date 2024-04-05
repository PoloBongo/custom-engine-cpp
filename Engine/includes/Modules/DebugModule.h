#pragma once
#include <iostream>

#include "Module.h"

class DebugModule : public Module
{
	public:
		virtual ~DebugModule();

		enum class TypeLog
		{
			INFO,
			WARNING,
			ERROR,
			SUCCESS
		};

		static void Log(const std::string& _message, const TypeLog _level = TypeLog::INFO)
		{
			std::string prefix_type;
			std::string color_code;
			switch (_level)
			{
				case TypeLog::INFO:
					prefix_type = "[INFO]";
					color_code = "\033[1;34m"; // Bleu
					break;
				case TypeLog::WARNING:
					prefix_type = "[WARNING]";
					color_code = "\033[1;33m"; // Jaune
					break;
				case TypeLog::ERROR:
					prefix_type = "[ERROR]";
					color_code = "\033[1;31m"; // Rouge
					break;
				case TypeLog::SUCCESS:
					prefix_type = "[SUCCESS]";
					color_code = "\033[1;32m"; // Vert
					break;
				default:
					break;
			}

			const std::string reset_color = "\033[0m"; // Reset la couleur
			// TODO : question pour Adrien : '\n' à la place de std::endl
			std::cout << color_code << prefix_type << " : " << _message << reset_color << std::endl;
		}

		static void LogError(const std::string& _message)
		{
			Log(_message, TypeLog::ERROR);
		}

		static void LogWarning(const std::string& _message)
		{
			Log(_message, TypeLog::WARNING);
		}

		static void LogInfo(const std::string& _message)
		{
			Log(_message, TypeLog::INFO);
		}

		static void LogSuccess(const std::string& _message)
		{
			Log(_message, TypeLog::SUCCESS);
		}

		/**
			* @brief Initialise le module.
			*/
		void Init() override;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;
};
