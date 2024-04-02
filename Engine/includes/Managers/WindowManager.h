#pragma once
#include "Module.h"

#include "lve_window.h"
#include "lve_device.h"
#include "lve_renderer.h"
#include "lve_game_object.h"
#include "lve_descriptors.h"

#include "Camera/lve_camera.h"   
#include "keyboard_movement_controller.h"   
#include "lve_buffer.h"   
#include "Systems/simple_render_system.h"   
#include "Systems/point_light_system.h"

//std
#include <memory>
#include <vector>
#include <chrono>
#include <iostream>
#include <string>
#include <map>


/**
 * @brief Classe WindowManager.
 *
 * Cette classe représente le gestionnaire de fenêtres dans le système.
 * Elle hérite de la classe Module, ce qui lui permet d'être intégrée dans le système de modules.
 * Le WindowManager est responsable de la gestion et de la manipulation des fenêtres de l'application.
 */
class WindowManager final : public Module
{
public:
	static constexpr int WIDTH = 800; ///< Largeur de la fenêtre par défaut.
	static constexpr int HEIGHT = 600; ///< Hauteur de la fenêtre par défaut.

	void Init() override;
	void Start() override;
	void FixedUpdate() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void RenderGui() override;
	void PostRender() override;
	void Release() override;

	lve::LveWindow* GetWindow() { return &lveWindow; }

private:
	void LoadGameObjects();

	lve::LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; /// Fenêtre de l'application.
	lve::LveDevice lveDevice{ lveWindow };
	lve::LveRenderer lveRenderer{ lveWindow, lveDevice };

	lve::LveDescriptorPool::Builder builder{ lveDevice };

	std::unique_ptr<lve::LveDescriptorSetLayout, std::default_delete<lve::LveDescriptorSetLayout>> *globalSetLayout;

	lve::SimpleRenderSystem* simpleRenderSystem;
	lve::PointLightSystem* pointLightSystem;
	lve::LveCamera* camera;

	std::chrono::steady_clock::time_point currentTime;
	lve::KeyboardMovementController cameraController{};
	lve::LveGameObject* viewerObject;

	std::vector<vk::DescriptorSet> globalDescriptorSets;
	std::vector<std::unique_ptr<lve::LveBuffer>> uboBuffers;

	// note : order of declarations matters
	std::unique_ptr<lve::LveDescriptorPool> globalPool{};
	lve::LveGameObject::Map* gameObjects;
};