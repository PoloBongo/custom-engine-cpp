#pragma once
#pragma once
#include "Module.h"

class RHIModule : public Module
{
public:
    virtual ~RHIModule() {}

	/**
	 * @brief Initialise le module.
	 */
	virtual void Init() override = 0;

	/**
	 * @brief Démarre le module.
	 */
	virtual void Start() override = 0;

	/**
	 * @brief Effectue une mise à jour fixe du module.
	 */
	virtual void FixedUpdate() override = 0;

	/**
	 * @brief Met à jour le module.
	 */
	virtual void Update() override = 0;

	/**
	 * @brief Fonction pré-rendu du module.
	 */
	virtual void PreRender()override = 0;

	/**
	 * @brief Rendu du module.
	 */
	virtual void Render()override = 0;

	/**
	 * @brief Rendu de l'interface graphique du module.
	 */
	virtual void RenderGui() override = 0;

	/**
	 * @brief Fonction post-rendu du module.
	 */
	virtual void PostRender()override = 0;

	/**
	 * @brief Libère les ressources utilisées par le module.
	 */
	virtual void Release() override = 0;

	/**
	 * @brief Finalise le module.
	 */
	virtual void Finalize() override = 0;


};
