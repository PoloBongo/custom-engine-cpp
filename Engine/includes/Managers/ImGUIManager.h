#pragma once

#include <functional>

#include "Module.h"
//#include "TimeModule.h"
#include "Managers/WindowManager.h"
#include "Managers/SceneManager.h"


class ImGuiManager final : public Module
{
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void Render() override;
	void Finalize() override;

	//TimeModule* timeModule = nullptr;

private:
	vk::Device device;
	vk::Queue graphicsQueue;

	WindowManager* windowManager = nullptr;
	SceneManager* sceneManager = nullptr;

	~ImGuiManager() = default;

	vk::Fence _immFence;
	vk::CommandBuffer _immCommandBuffer;
	vk::CommandPool _immCommandPool;

	void immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function);

	GameObject* selectedGameObject = nullptr; // Entité sélectionnée
	std::map<BaseScene*, int> selectedEntityIndices; // Index de l'entité sélectionnée
	bool isRenamePopupOpen = false; // État de la fenêtre de renommage
	int entityToRename = -1; // Index de l'entité à renommer
	char renameBuffer[256]; // Buffer pour le nouveau nom de l'entité
	int sceneToRename = -1;
	char renameSceneBuffer[100];

	bool openPositionEdit = false;
	bool openRotationEdit = false;
	bool openScaleEdit = false;
	glm::vec3 positionEdit;
	float rotationEdit;
	glm::vec3 scaleEdit;

	void GetGUI();

	void DrawHierarchy();
	void DrawInspector();
	void DisplayTransform(Transform* _transform);

	void ShowRenamePopup();
	void RenameGameObject(GameObject* _gameObject, const std::string& _newName);
	void DeleteGameObject(GameObject* _gameObject);
	void DuplicateGameObject(int _index);
};