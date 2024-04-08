#include "Managers/ImGUIManager.h"
#include "ModuleManager.h"
#include "GAmeObject/GameObject.h"
#include "lve_renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

void ImGuiManager::Init()
{
	windowManager = moduleManager->GetModule<WindowManager>();
	sceneManager = moduleManager->GetModule<SceneManager>();

	device = windowManager->GetDevice()->device();
	graphicsQueue = windowManager->GetDevice()->graphicsQueue();
	lve::QueueFamilyIndices queueFamilyIndices = windowManager->GetDevice()->findPhysicalQueueFamilies();

	// Création du pool de commandes
	vk::CommandPoolCreateInfo commandPoolInfo(
		vk::CommandPoolCreateFlags(), // Flags de création
		queueFamilyIndices.graphicsFamily // Indice de la famille de file d'attente de commandes
	);
	_immCommandPool = device.createCommandPool(commandPoolInfo);

	// Allocation du tampon de commande pour les soumissions immédiates
	vk::CommandBufferAllocateInfo cmdAllocInfo(
		_immCommandPool, // Pool de commandes
		vk::CommandBufferLevel::ePrimary, // Niveau du tampon de commande
		1 // Nombre de tampons à allouer
	);
	_immCommandBuffer = device.allocateCommandBuffers(cmdAllocInfo)[0];

	// Ajout de la fonction de suppression du pool de commandes à la file de suppression principale
	//_mainDeletionQueue.push_back([=]() {
	//	device.destroyCommandPool(_immCommandPool);
	//});
}

void ImGuiManager::Start()
{
	Module::Start();

	//timeModule = moduleManager->GetModule<TimeModule>();

	//ImGui::CreateContext();

	vk::DescriptorPoolSize pool_sizes[] = { { vk::DescriptorType::eSampler, 1000 },
	{ vk::DescriptorType::eCombinedImageSampler, 1000 },
	{ vk::DescriptorType::eSampledImage, 1000 },
	{ vk::DescriptorType::eStorageImage, 1000 },
	{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
	{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
	{ vk::DescriptorType::eUniformBuffer, 1000 },
	{ vk::DescriptorType::eStorageBuffer, 1000 },
	{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
	{ vk::DescriptorType::eStorageBufferDynamic , 1000 },
	{ vk::DescriptorType::eInputAttachment, 1000 } };

	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
	pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	vk::DescriptorPool imguiPool;
	if (windowManager->GetDevice()->device().createDescriptorPool(&pool_info, nullptr, &imguiPool) != vk::Result::eSuccess) {
		throw std::runtime_error("Impossible de creer la pool de imgui!");
	}

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplGlfw_InitForVulkan(windowManager->GetWindow()->GetGLFWwindow(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = windowManager->GetDevice()->GetInstance();
	init_info.PhysicalDevice = windowManager->GetDevice()->GetPhysicalDevice();
	init_info.Device = device;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.RenderPass = windowManager->GetRenderer()->GetSwapChainRenderPass();
	//init_info.UseDynamicRendering = VK_TRUE;
	//init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = (VkSampleCountFlagBits)vk::SampleCountFlagBits::e1;

	ImGui_ImplVulkan_Init(&init_info);

	// execute a gpu command to upload imgui font textures
	//immediate_submit([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

	// clear font textures from cpu data
	//ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiManager::immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function)
{
	vk::Device device = windowManager->GetDevice()->device();
	vk::Queue graphicsQueue = windowManager->GetDevice()->graphicsQueue();

	device.resetFences(_immFence);
	_immCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	vk::CommandBuffer cmd = _immCommandBuffer;

	vk::CommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	cmd.begin(cmdBeginInfo);

	function(cmd);

	cmd.end();

	vk::CommandBufferSubmitInfo cmdinfo{};
	cmdinfo.sType = vk::StructureType::eCommandBufferSubmitInfo;
	cmdinfo.pNext = nullptr;
	cmdinfo.commandBuffer = cmd;
	cmdinfo.deviceMask = 0;

	vk::SubmitInfo2 submitInfo{};
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmdinfo;
	vk::DispatchLoaderDynamic dispatcher = vk::DispatchLoaderDynamic();
	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	graphicsQueue.submit2KHR(submitInfo, _immFence, dispatcher);

	device.waitForFences(_immFence, VK_TRUE, 9999999999);
}

void ImGuiManager::Update()
{
	Module::Update();

	//ImGui_ImplGlfw_ProcessEvent(&e);

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	GetGUI();

	//imgui commands
}

void ImGuiManager::Render()
{
	Module::Render();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), windowManager->GetRenderer()->GetCurrentCommandBuffer());

}

void ImGuiManager::Finalize()
{
	Module::Finalize();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}

void ImGuiManager::GetGUI() {

	DrawHierarchy();
	DrawInspector();
}



void ImGuiManager::DrawInspector() {
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Inspector");

	if (selectedEntityIndex >= 0 && selectedEntityIndex < sceneManager->GetMainScene()->GetRootObjects().size()) {
		GameObject* gameObject = sceneManager->GetMainScene()->GetRootObjects()[selectedEntityIndex];
		Transform* transform = gameObject->GetTransform();

		// Afficher le nom du GameObject
		ImGui::Text("Name: ");
		ImGui::SameLine();
		ImGui::SetWindowFontScale(1.2f);
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), gameObject->GetName().c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Bouton pour afficher la popup de renommage
		ImGui::SameLine();
		if (ImGui::Button("Rename")) {
			isRenamePopupOpen = true;
			entityToRename = selectedEntityIndex;
			strncpy_s(renameBuffer, gameObject->GetName().c_str(), sizeof(renameBuffer) - 1);
			renameBuffer[sizeof(renameBuffer) - 1] = '\0';
			ImGui::OpenPopup("Rename Entity");
		}
		ShowRenamePopup();

		// Header Transform
		if (ImGui::CollapsingHeader("Transform")) {
			// Variables pour la modification manuelle
			bool openPositionEdit = false, openRotationEdit = false, openScaleEdit = false;
			glm::vec3 positionEdit, scaleEdit;
			float rotationEdit;

			// Position
			glm::vec3 position = gameObject->GetPosition();
			if (ImGui::Button("...##position")) {
				openPositionEdit = true;
				positionEdit = position;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat3("Position", &position[0])) {
				gameObject->SetPosition(position);
			}
			// Rotation
			float rotation = gameObject->GetRotation();
			if (ImGui::Button("...##rotation")) {
				openRotationEdit = true;
				rotationEdit = rotation;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Rotation", &rotation)) {
				gameObject->SetRotation(rotation);
			}
			// Scale
			glm::vec3 scale = gameObject->GetScale();
			if (ImGui::Button("...##scale")) {
				openScaleEdit = true;
				scaleEdit = scale;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat3("Scale", &scale[0])) {
				gameObject->SetScale(scale);
			}

			// Popups pour la saisie manuelle
			if (openPositionEdit) {
				ImGui::OpenPopup("Edit Position");
				if (ImGui::BeginPopupModal("Edit Position")) {
					ImGui::InputFloat3("New Position", &positionEdit[0]);
					if (ImGui::Button("OK##Position")) {
						gameObject->SetPosition(positionEdit);
						openPositionEdit = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel##Position")) {
						openPositionEdit = false;
					}
					ImGui::EndPopup();
				}
			}
			if (openRotationEdit) {
				ImGui::OpenPopup("Edit Rotation");
				if (ImGui::BeginPopupModal("Edit Rotation")) {
					ImGui::InputFloat("New Rotation", &rotationEdit);
					if (ImGui::Button("OK##Rotation")) {
						gameObject->SetRotation(rotationEdit);
						openRotationEdit = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel##Rotation")) {
						openRotationEdit = false;
					}
					ImGui::EndPopup();
				}
			}
			if (openScaleEdit) {
				ImGui::OpenPopup("Edit Scale");
				if (ImGui::BeginPopupModal("Edit Scale")) {
					ImGui::InputFloat3("New Scale", &scaleEdit[0]);
					if (ImGui::Button("OK##Scale")) {
						gameObject->SetScale(scaleEdit);
						openScaleEdit = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel##Scale")) {
						openScaleEdit = false;
					}
					ImGui::EndPopup();
				}
			}
		}

		// Bouton pour ajouter un composant
		if (ImGui::Button("Add Component")) {
			// Logique d'ajout de composant (à implémenter)
		}

	}
	else {
		ImGui::Text("No GameObject selected");
	}

	ImGui::End();
}



void ImGuiManager::DrawHierarchy() {
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Hierarchy");

	// Creation d'un nouvel objet
	if (ImGui::Button("New GameObject")) {
		if (sceneManager->GetScene("Default") != nullptr) {
			sceneManager->GetScene("Default")->CreateGameObject();
			std::cout << "Added new GameObject." << std::endl;
		}
		else {
			std::cout << "No scene found." << std::endl;
		}
	}

	// Barre de recherche
	static char searchBuffer[100];
	ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));

	// Liste des objets de la scène
	auto& gameObjects = sceneManager->GetMainScene()->GetRootObjects();

	for (size_t i = 0; i < gameObjects.size(); ++i) {
		GameObject* gameObject = gameObjects[i];

		// Appliquer le filtre de recherche
		if (strstr(gameObject->GetName().c_str(), searchBuffer)) {
			// Afficher l'entité comme une option sélectionnable
			bool isSelected = (selectedEntityIndex == i);
			ImGui::PushID(i);

			if (ImGui::Selectable(gameObject->GetName().c_str(), isSelected)) {
				selectedEntityIndex = i;
			}

			// Construire le menu contextuel
			if (ImGui::BeginPopupContextItem()) {
				// Option de renommage
				if (ImGui::MenuItem("Rename")) {
					isRenamePopupOpen = true;
					entityToRename = i;
					strncpy_s(renameBuffer, gameObjects[i]->GetName().c_str(), sizeof(renameBuffer) - 1);
					renameBuffer[sizeof(renameBuffer) - 1] = '\0';
					ImGui::OpenPopup("Rename Entity");
				}	ShowRenamePopup();

				ImGui::Separator();
				// Option de suppression
				if (ImGui::MenuItem("Delete")) {
					DeleteGameObject(i);
				}

				ImGui::Separator();
				// Option de duplication
				if (ImGui::MenuItem("Duplicate")) {
					DuplicateGameObject(i);
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void ImGuiManager::ShowRenamePopup() {
	if (ImGui::BeginPopup("Rename Entity")) {
		ImGui::InputText("##edit", renameBuffer, IM_ARRAYSIZE(renameBuffer));
		if (ImGui::Button("OK##Rename")) {
			RenameGameObject(entityToRename, std::string(renameBuffer));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##Rename")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ImGuiManager::RenameGameObject(int _index, const std::string& _newName) {
	auto& gameObjects = sceneManager->GetMainScene()->GetRootObjects();
	if (_index >= 0 && _index < gameObjects.size()) {
		gameObjects[_index]->SetName(_newName);
	}
}

void ImGuiManager::DeleteGameObject(int _index) {
	auto& gameObjects = sceneManager->GetMainScene()->GetRootObjects();
	if (_index >= 0 && _index < gameObjects.size()) {
		sceneManager->GetMainScene()->DestroyGameObject(gameObjects[_index]);
		gameObjects.erase(gameObjects.begin() + _index);
	}
}

void ImGuiManager::DuplicateGameObject(int _index) {
	auto& gameObjects = sceneManager->GetMainScene()->GetRootObjects();
	if (_index < gameObjects.size()) {
		//sceneManager->GetMainScene()->CloneGameObject(gameObjects[_index]);
	}
}
