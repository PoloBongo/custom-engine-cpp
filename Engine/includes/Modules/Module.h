#pragma once

class ModuleManager;

/**
 * @brief Classe Module.
 *
 * Cette classe représente un module de l'application.
 * Les modules peuvent être initialisés, démarrés, mis à jour et libérés de manière contrôlée par le ModuleManager.
 */
class Module
{
	friend class ModuleManager;

	protected:
		/**
		 * @brief Destructeur protégé.
		 *
		 * Le destructeur est protégé car les modules doivent être détruits uniquement par le ModuleManager.
		 */
		~Module() = default;

	public:
		/**
		 * @brief Initialise le module.
		 */
		virtual void Init()
		{
		}

		/**
		 * @brief Démarre le module.
		 */
		virtual void Start()
		{
		}

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		virtual void FixedUpdate()
		{
		}

		/**
		 * @brief Met à jour le module.
		 */
		virtual void Update()
		{
		}

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		virtual void PreRender()
		{
		}

		/**
		 * @brief Rendu du module.
		 */
		virtual void Render()
		{
		}

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		virtual void RenderGui()
		{
		}

		/**
		 * @brief Fonction post-rendu du module.
		 */
		virtual void PostRender()
		{
		}

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		virtual void Release()
		{
		}

		/**
		 * @brief Finalise le module.
		 */
		virtual void Finalize()
		{
		}

	protected:
		ModuleManager* moduleModule = nullptr; /**< Pointeur vers le gestionnaire de modules. */
};
