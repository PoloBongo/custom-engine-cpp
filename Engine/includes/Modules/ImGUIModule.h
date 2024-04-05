#pragma once

#include <functional>

#include "Module.h"
//#include "TimeModule.h"
#include "Modules/WindowModule.h"


class ImGuiModule final : public Module
{
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

	void GetGui();

	//TimeModule* timeModule = nullptr;

	vk::Fence         immFence;
	vk::CommandBuffer immCommandBuffer;
	vk::CommandPool   immCommandPool;

	void ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function);

	protected:
		vk::Device device;
		vk::Queue  graphicsQueue;

		WindowModule* windowModule = nullptr;

		~ImGuiModule() = default;
};
