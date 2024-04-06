#pragma once
#pragma once
#include "Modules/Module.h"

class RHIModule : public Module
{
	public:
		virtual ~RHIModule()
		= default;

		/**
		 * @brief Initialise le module.
		 */
		void Init() override = 0;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override = 0;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override = 0;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override = 0;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override = 0;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override = 0;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override = 0;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override = 0;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override = 0;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override = 0;
};
