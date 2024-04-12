#pragma once
#include "Module.h"

#include "lve_descriptors.h"
#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_renderer.h"
#include "lve_window.h"
#include "lve_texture.h"

#include "keyboard_movement_controller.h"
#include "lve_buffer.h"
#include "Camera/lve_camera.h"
#include "Systems/point_light_system.h"
#include "Systems/simple_render_system.h"

//std
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "GameObject/GameObject.h"


/**
 * @brief Classe WindowModule.
 *
 * Cette classe repr�sente le gestionnaire de fen�tres dans le syst�me.
 * Elle h�rite de la classe Module, ce qui lui permet d'�tre int�gr�e dans le syst�me de modules.
 * Le WindowModule est responsable de la gestion et de la manipulation des fen�tres de l'application.
 */
class WindowModule final : public Module
{
	public:
		static constexpr int WIDTH  = 800; ///< Largeur de la fen�tre par d�faut.
		static constexpr int HEIGHT = 600; ///< Hauteur de la fen�tre par d�faut.

		void Init() override;
		void Start() override;
		void FixedUpdate() override;
		void Update() override;
		void PreRender() override;
		void Render() override;
		void RenderGui() override;
		void PostRender() override;
		void Release() override;
		void Finalize() override;

		lve::LveWindow*   GetWindow() const { return lveWindow; }

	private:

		lve::LveWindow*   lveWindow; /// Fen�tre de l'application.
};
