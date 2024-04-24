#include "Modules/ImGUIModule.h"
#include "lve_renderer.h"
#include "ModuleManager.h"
#include "Modules/WindowModule.h"
#include "Modules/ImGUIModule.h"

#include <CoreEngine.h>

#include "ImGUIInterface.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Light.h"
#include "rhi.h"

#include "GameObject/PreGameObject/LightGameObject.h"
#include "Transform.h"
#include "Scene/SceneManager.h"
#include "TCP/Errors.h"

#include <iostream>
#include <fstream>

class BaseScene;
// ----------========== IMGUI SETTINGS ==========---------- //

void ImGuiModule::Init()
{
	Module::Init();

	windowModule = moduleManager->GetModule<WindowModule>();
	rhiModule = moduleManager->GetModule<RHIModule>();
	sceneManager = moduleManager->GetModule<SceneManager>();
	device = rhiModule->GetDevice()->Device();
	graphicsQueue = rhiModule->GetDevice()->GraphicsQueue();
	const lve::QueueFamilyIndices queue_family_indices = rhiModule->GetDevice()->FindPhysicalQueueFamilies();

	// Création du pool de commandes
	const vk::CommandPoolCreateInfo command_pool_info(
		vk::CommandPoolCreateFlags(),       // Flags de création
		queue_family_indices.graphicsFamily // Indice de la famille de file d'attente de commandes
	);
	immCommandPool = device.createCommandPool(command_pool_info);

	// Allocation du tampon de commande pour les soumissions immédiates
	const vk::CommandBufferAllocateInfo cmd_alloc_info(
		immCommandPool,                   // Pool de commandes
		vk::CommandBufferLevel::ePrimary, // Niveau du tampon de commande
		1                                 // Nombre de tampons à allouer
	);
	immCommandBuffer = device.allocateCommandBuffers(cmd_alloc_info)[0];

	// Ajout de la fonction de suppression du pool de commandes à la file de suppression principale
	//_mainDeletionQueue.push_back([=]() {
	//	device.destroyCommandPool(_immCommandPool);
	//});
}

void ImGuiModule::Start()
{
	Module::Start();
	//timeModule = moduleManager->GetModule<TimeModule>();

	//ImGui::CreateContext();

	const vk::DescriptorPoolSize pool_sizes[] = {
		{vk::DescriptorType::eSampler, 1000},
		{vk::DescriptorType::eCombinedImageSampler, 1000},
		{vk::DescriptorType::eSampledImage, 1000},
		{vk::DescriptorType::eStorageImage, 1000},
		{vk::DescriptorType::eUniformTexelBuffer, 1000},
		{vk::DescriptorType::eStorageTexelBuffer, 1000},
		{vk::DescriptorType::eUniformBuffer, 1000},
		{vk::DescriptorType::eStorageBuffer, 1000},
		{vk::DescriptorType::eUniformBufferDynamic, 1000},
		{vk::DescriptorType::eStorageBufferDynamic, 1000},
		{vk::DescriptorType::eInputAttachment, 1000}
	};

	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
	pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
	pool_info.pPoolSizes = pool_sizes;

	vk::DescriptorPool im_gui_pool;
	if (rhiModule->GetDevice()->Device().createDescriptorPool(&pool_info, nullptr, &im_gui_pool) !=
		vk::Result::eSuccess)
		throw std::runtime_error("Impossible de creer la pool de imgui!");

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplGlfw_InitForVulkan(windowModule->GetGlfwWindow(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = rhiModule->GetDevice()->GetInstance();
	init_info.PhysicalDevice = rhiModule->GetDevice()->GetPhysicalDevice();
	init_info.Device = device;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = im_gui_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.RenderPass = rhiModule->GetRenderer()->GetSwapChainRenderPass();
	//init_info.UseDynamicRendering = VK_TRUE;
	//init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = static_cast<VkSampleCountFlagBits>(vk::SampleCountFlagBits::e1);

	ImGui_ImplVulkan_Init(&init_info);

	// execute a gpu command to upload imgui font textures
	//immediate_submit([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

	// clear font textures from cpu data
	//ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void ImGuiModule::Update()
{
	Module::Update();

	// Mise à jour d'ImGui
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	GetGui();
}

void ImGuiModule::PreRender()
{
	Module::PreRender();
}

void ImGuiModule::Render()
{
	Module::Render();

	// Rendu d'ImGui
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), rhiModule->GetRenderer()->GetCurrentCommandBuffer());
}

void ImGuiModule::RenderGui()
{
	Module::RenderGui();
}

void ImGuiModule::PostRender()
{
	Module::PostRender();
}

void ImGuiModule::Release()
{
	Module::Release();
}

void ImGuiModule::Finalize()
{
	Module::Finalize();


	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}

void ImGuiModule::ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const
{
	const vk::Device device = rhiModule->GetDevice()->Device();
	const vk::Queue  graphics_queue = rhiModule->GetDevice()->GraphicsQueue();

	device.resetFences(immFence);
	immCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	const vk::CommandBuffer cmd = immCommandBuffer;

	vk::CommandBufferBeginInfo cmd_begin_info{};
	cmd_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmd_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	cmd.begin(cmd_begin_info);

	_function(cmd);

	cmd.end();

	vk::CommandBufferSubmitInfo cmd_info;
	cmd_info.sType = vk::StructureType::eCommandBufferSubmitInfo;
	cmd_info.pNext = nullptr;
	cmd_info.commandBuffer = cmd;
	cmd_info.deviceMask = 0;

	vk::SubmitInfo2 submitInfo{};
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmd_info;
	constexpr auto dispatcher = vk::DispatchLoaderDynamic();
	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	graphics_queue.submit2KHR(submitInfo, immFence, dispatcher);

	if (device.waitForFences(immFence, VK_TRUE, 9999999999) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed GUI");
	}
}


// ----------========== IMGUI SHOWN ==========---------- //

void ImGuiModule::GetGui()
{
	ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;

	// Flags pour les fenêtres déplaçables et non-redimensionnables
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

	// Dessin de la fenêtre "Hierarchy" - Gauche
	ImGui::SetNextWindowSize(ImVec2(300, mainWindowSize.y), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Ancrage en haut à gauche
	ImGui::Begin("Hierarchy", nullptr, window_flags);
	DrawHierarchyWindow();
	ImGui::End();

	// Dessin de la fenêtre "Inspector" - Droite
	ImGui::SetNextWindowSize(ImVec2(300, mainWindowSize.y), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(mainWindowSize.x - 300, 0), ImGuiCond_Always); // Ancrage en haut à droite
	ImGui::Begin("Inspector", nullptr, window_flags);
	DrawInspectorWindow();
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(170, 79), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(mainWindowSize.x / 2 - 200 / 2, 0), ImGuiCond_Always); // Ancrage en haut à droite
	ImGui::Begin("Modes", nullptr, window_flags);
	DrawModesWindow();
	ImGui::End();

	DrawSettingsWindow();
}

void ImGuiModule::AnchorWindow(const std::string& _windowName)
{
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 mainWindowPos = ImGui::GetMainViewport()->Pos;
	ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;

	// Ancrage à droite avec mise à jour dynamique lors du redimensionnement
	ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowSize.x - 300, windowPos.y));

	// Ancrage à gauche ou à droite si nécessaire
	if (windowPos.x < mainWindowPos.x + 50) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowPos.x, windowPos.y));
	}
	else if (windowPos.x > mainWindowPos.x + mainWindowSize.x - 350) {
		// S'assurer que la fenêtre reste collée au bord droit même après redimensionnement
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowPos.x + mainWindowSize.x - 300, windowPos.y));
	}

	// Correction pour éviter le débordement par le haut ou par le bas
	if (windowPos.y < mainWindowPos.y) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(windowPos.x, mainWindowPos.y));
	}
	else if (windowPos.y + mainWindowSize.y > mainWindowPos.y + mainWindowSize.y) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(windowPos.x, mainWindowPos.y + mainWindowSize.y - mainWindowSize.y));
	}
}


// ----------========== DRAW WINDOWS ==========---------- //

void ImGuiModule::DrawInspectorWindow() {
	// Vérifier si un GameObject est sélectionné
	if (selectedGameObject) {
		// Affichage du nom du GameObject
		ImGui::Text("Name: ");
		ImGui::SameLine();
		ImGui::SetWindowFontScale(1.2f);
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), selectedGameObject->GetName().c_str());
		
		ImGui::SetWindowFontScale(1.0f);

		// Bouton pour afficher la popup de renommage
		ImGui::SameLine();
		if (ImGui::Button("Rename")) {
			isRenamePopupOpen = true;
			strncpy_s(renameBuffer, selectedGameObject->GetName().c_str(), sizeof(renameBuffer) - 1);
			renameBuffer[sizeof(renameBuffer) - 1] = '\0';
			ImGui::OpenPopup("Rename Entity");
		}
		ShowRenamePopup();

		// Affichage des propriétés de transformation
		if (ImGui::CollapsingHeader("Transform")) {
			DisplayTransform(selectedGameObject->GetTransform());
		}

		if (ImGui::CollapsingHeader("Texture")) {
			int texture = selectedGameObject->GetTexture();
			if (ImGui::InputInt("Texture", &texture)) {

				if (texture < 0) {
					texture = 0;
				}
				else if (texture > moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors().size() - 1) {
					texture -= 1;
				}
				else {
					selectedGameObject->SetTexture(texture);
				}
			}
			float textureMulti = selectedGameObject->GetTexMultiplier();
			if (ImGui::DragFloat("Texture Multiplier", &textureMulti, 1, 0.0, 100.0)) {
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					selectedGameObject->SetTexMultiplier(textureMulti);
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier());
					selectedGameObject->GetModel() = lve_model;
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
				}
			}
			//ImGui::SameLine();
			if (ImGui::Button("Reset Multiplier", ImVec2(150, 20))) {
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					selectedGameObject->SetTexMultiplier(1.0f);
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier());
					selectedGameObject->GetModel() = lve_model;
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
				}
			}

			if (ImGui::Checkbox("Texture Viewer (0.3v)", &textureView)) {
				!textureView;
			}

			if (textureView) {
				int sizeDescList = moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors().size();

				// Vulkan crie, parce que ce sont les descriptors d'un autre pool et d'une autre pipeline
				if (selectedGameObject->GetTexture() > sizeDescList - 1) {
					ImGui::Image(moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors()[0]->at(0), ImVec2(200, 200));
				}
				else {
					ImGui::Image(moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors()[selectedGameObject->GetTexture()]->at(0), ImVec2(200, 200));

				}
			}
		}



		// Detection de Light et affichage des proprietes de la lumiere
		Light* lightComponent = selectedGameObject->GetComponent<Light>();
		if (lightComponent) {
			// Intensite de la lumiere
			float intensity = lightComponent->lightIntensity;
			if (ImGui::DragFloat("Light Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
				lightComponent->lightIntensity = intensity;
			}

			// Couleur de la lumiere
			glm::vec3 color = selectedGameObject->GetColor();
			if (ImGui::ColorEdit3("Color", glm::value_ptr(color))) {
				selectedGameObject->SetColor(color);
			}
		}

		// Bouton pour ajouter un composant
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponentPopup");
		}

		// Popup pour l'ajout de composant
		if (ImGui::BeginPopup("AddComponentPopup")) {
			if (ImGui::MenuItem("Add Light")) {
				Light* newLight = selectedGameObject->CreateComponent<Light>();
				newLight->lightIntensity = 1.0;  // Intensit� initiale standard
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	else {
		ImGui::Text("No GameObject selected");
	}
}

void ImGuiModule::DrawHierarchyWindow() {
	// Bouton pour créer un nouveau GameObject
	if (ImGui::Button("New GameObject")) {
		ImGui::OpenPopup("CreateGameObjectPopup");
	}
	ImGui::SameLine();
	// Bouton pour ajouter une nouvelle scène
	if (ImGui::Button("Add New Scene")) {
		sceneManager->CreateScene("New Scene", false);
	}
	if (ImGui::BeginPopup("CreateGameObjectPopup")) {
		if (ImGui::MenuItem("Cube")) {
			CreateSpecificGameObject(GameObjectType::Cube);
			std::cout << "Added new GameObject-Cube to current scene." << std::endl;

		}
		if (ImGui::MenuItem("Light")) {
			CreateSpecificGameObject(GameObjectType::Light);
			std::cout << "Added new GameObject-Light to current scene." << std::endl;

		}
		if (ImGui::MenuItem("Plane")) {
			CreateSpecificGameObject(GameObjectType::Plane);
			std::cout << "Added new GameObject-Plane to current scene." << std::endl;

		}
		ImGui::EndPopup();
	}

	// Barre de recherche
	static char searchBuffer[100];
	ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));

	// Affichage des scènes et de leurs GameObjects
	const auto& scenes = sceneManager->GetScenes();
	for (size_t i = 0; i < scenes.size(); ++i) {
		ImGui::Spacing();
		ImGui::Separator();

		const auto& scene = scenes[i];
		bool isCurrentScene = (sceneManager->GetCurrentScene() == scene.get());

		ImGui::PushID(i); // Identifiant unique pour chaque scène

		auto SceneTree = ImGui::TreeNode(scene->GetName().c_str());

		// Menu contextuel pour chaque scène
		if (ImGui::BeginPopupContextItem("Scene Menu")) {
			if (ImGui::MenuItem("Set Active")) {
				sceneManager->SetCurrentScene(static_cast<int>(i)); // Définit la scène courante
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Rename")) {
				sceneToRename = i;
				strncpy_s(renameSceneBuffer, scene->GetName().c_str(), sizeof(renameSceneBuffer));
				renameSceneBuffer[sizeof(renameSceneBuffer) - 1] = '\0';
				ImGui::OpenPopup("Rename Scene");
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Delete")) {
				sceneManager->DestroyScene(scene->GetName()); // Supprime la scène
			}
			ImGui::EndPopup();
		}

		// Affichage du nom de la scène avec un bouton "Set Active" si nécessaire
		if (SceneTree) {
			// Bouton pour définir la scène active
			if (!isCurrentScene) {
				ImGui::SameLine(ImGui::GetWindowWidth() - 100); // Décalage à droite
				std::string buttonLabel = "Set Active##" + std::to_string(i);
				if (ImGui::Button(buttonLabel.c_str())) {
					sceneManager->SetCurrentScene(static_cast<int>(i));
				}
			}

			// Affichage des GameObjects
			const auto& gameObjects = scene->GetRootObjects();
			for (size_t j = 0; j < gameObjects.size(); ++j) {
				const auto& gameObject = gameObjects[j];

				ImGui::PushID(j); // Identifiant unique pour chaque GameObject

				if (strstr(gameObject->GetName().c_str(), searchBuffer)) {
					if (ImGui::Selectable(gameObject->GetName().c_str(), selectedGameObject == gameObject)) {
						selectedGameObject = gameObject;
					}
					if (j == gameObjects.size() - 1) {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
					}

					// Menu contextuel pour GameObject
					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Rename")) {
							isRenamePopupOpen = true;
							entityToRename = j;
							strncpy_s(renameBuffer, gameObject->GetName().c_str(), sizeof(renameBuffer) - 1);
							renameBuffer[sizeof(renameBuffer) - 1] = '\0';
							ImGui::OpenPopup("Rename Entity");
							selectedGameObject = gameObject;
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Delete")) { DeleteGameObject(selectedGameObject); }

						ImGui::Separator();
						if (ImGui::MenuItem("Duplicate")) {}

						ImGui::EndPopup();
					}
					ImGui::PopID();  // Restaure l'ID précédent pour les GameObjects
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();  // Restaure l'ID précédent pour les scènes
	}
}

void ImGuiModule::DrawModesWindow() {

	//std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;

	if (ImGui::Button("Play Button", ImVec2(150, 20))) {
		if (Engine::GetInstance()->GetEngineMode() == EngineMode::Editor) {
			Engine::GetInstance()->PlayEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
		else {
			Engine::GetInstance()->EditorEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
	}

	if (ImGui::Button("Pause Button", ImVec2(150, 20))) {
		if (Engine::GetInstance()->GetEngineMode() == EngineMode::Play) {
			Engine::GetInstance()->PauseEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
		else if (Engine::GetInstance()->GetEngineMode() == EngineMode::Pause) {
			Engine::GetInstance()->PlayEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
	}
}


void ImGuiModule::DrawSettingsWindow() {
	if (ImGui::Begin("Settings")) {
		ImGUIInterface::EditTheme();
		if (ImGui::CollapsingHeader("Input")) {
			// Input settings
		}
		if (ImGui::CollapsingHeader("Graphics")) {
			// Graphics settings
		}
		if (ImGui::CollapsingHeader("Audio")) {
			// Audio settings
		}
		if (ImGui::CollapsingHeader("Network")) {
			//Network settings
		}
	}
	ImGui::End();
}

void ImGuiModule::DisplayTransform(Transform* _transform) {
	if (!_transform) return;

	// Position
	glm::vec3 position = _transform->GetPosition();
	if (ImGui::DragFloat3("Position", &position[0])) {
		_transform->SetPosition(position);
	}

	// Rotation
	glm::vec3 rotationDegrees = _transform->GetRotationDegrees();
	if (ImGui::DragFloat3("Rotation", &rotationDegrees[0])) {
		_transform->SetRotationDegrees(rotationDegrees);
	}

	// Scale
	glm::vec3 scale = _transform->GetScale();
	if (ImGui::DragFloat3("Scale", &scale[0])) {
		_transform->SetScale(scale);
	}
}

// ----------========== POPUPS ==========---------- //

void ImGuiModule::ShowRenamePopup() {
	// Gestion de la fenêtre popup pour renommer un gameobject
	if (isRenamePopupOpen && selectedGameObject) {
		ImGui::OpenPopup("Rename Entity");
		if (ImGui::BeginPopup("Rename Entity")) {
			ImGui::InputText("##edit", renameBuffer, IM_ARRAYSIZE(renameBuffer));
			if (ImGui::Button("OK##Rename")) {
				RenameGameObject(selectedGameObject, std::string(renameBuffer));
				isRenamePopupOpen = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##Rename")) {
				isRenamePopupOpen = false;
			}
			ImGui::EndPopup();
		}
	}

	// Gestion de la fenêtre popup pour renommer la scène
	if (sceneToRename >= 0) {
		ImGui::OpenPopup("Rename Scene");
		if (ImGui::BeginPopup("Rename Scene")) {
			ImGui::InputText("New Name", renameSceneBuffer, IM_ARRAYSIZE(renameSceneBuffer));
			if (ImGui::Button("OK")) {
				const auto& scenes = sceneManager->GetScenes();
				if (sceneToRename < scenes.size()) {
					sceneManager->RenameScene(scenes[sceneToRename]->GetName(), renameSceneBuffer);
				}
				sceneToRename = -1;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				sceneToRename = -1;
			}
			ImGui::EndPopup();
		}
	}
}

// ----------========== RENAME / DELETE / DUPLICATE ==========---------- //

void ImGuiModule::RenameGameObject(GameObject* _gameObject, const std::string& _newName) {
	if (_gameObject) {
		std::cout << "Renamed GameObject: " << _gameObject->GetName() << " to " << _newName << std::endl;
		_gameObject->SetName(_newName);
	}
}
void ImGuiModule::DeleteGameObject(GameObject* _gameObject) {
	if (_gameObject) {
		BaseScene* currentScene = sceneManager->GetCurrentScene();
		if (currentScene) {
			currentScene->RemoveObject(_gameObject, true); // Suppression de l'objet
		}
	}
}
void ImGuiModule::DuplicateGameObject(GameObject* _gameObject) {
}

void ImGuiModule::CreateSpecificGameObject(GameObjectType _type) {
	BaseScene* currentScene = sceneManager->GetCurrentScene();
	if (currentScene) {
		GameObject* newGameObject = nullptr;
		switch (_type) {
		case GameObjectType::Cube:
			newGameObject = currentScene->CreateCubeGameObject();
			break;
		case GameObjectType::Light:
			newGameObject = currentScene->CreateLightGameObject();
			break;
		case GameObjectType::Plane:
			newGameObject = currentScene->CreatePlaneGameObject();
			break;
		}

		if (newGameObject) {
			currentScene->AddRootObject(newGameObject);
		}
	}
}