#pragma once

// std
#include <cstdint>
#include <stack>
#include <string>
#include <vector>

// Camera
#include "Camera/BaseCamera.h"

namespace lve
{
	class BaseCamera;

	// TODO: Make inherit from System
	class CameraManager final
	{
		public:
			CameraManager();

			void Initialize();
			void Destroy();
			void Update();
			void OnPostSceneChange();

			BaseCamera* CurrentCamera() const;

			void AddCamera(BaseCamera* _camera, bool _bSwitchTo = false);

			// Clears stack and pushes the given camera onto it, then returns a pointer to it
			BaseCamera* SetCamera(BaseCamera* _camera, bool _bAlignWithPrevious);
			BaseCamera* CycleCamera(uint32_t _deltaIndex, bool _bAlignWithPrevious = true);
			BaseCamera* SetCameraByName(const std::string& _name, bool _bAlignWithPrevious);

			BaseCamera* PushCamera(BaseCamera* _camera, bool _bAlignWithPrevious, bool _bInitialize);
			BaseCamera* PushCameraByName(const std::string& _name, bool _bAlignWithPrevious, bool _bInitialize);
			void        PopCamera(bool _bAlignWithCurrent = false);

			BaseCamera* GetCameraByName(const std::string& _name) const;

			void DrawImGuiObjects();

			// Copies position, rotation, and FOV of "from" to "to"
			void AlignCameras(const BaseCamera* _from, BaseCamera* _to);

		private:
			/*EventReply OnActionEvent(Action action, ActionEvent actionEvent);
			ActionCallback<CameraManager> m_ActionCallback;*/

			uint32_t GetCameraIndex(const BaseCamera* _camera) const;

			// TODO: Roll custom stack class
			// Stack containing temporary cameras, the topmost of which is the current camera
			// Always contains at least one element
			std::stack<BaseCamera*> cameraStack;
			// All cameras, unordered
			std::vector<BaseCamera*> cameras;

			bool m_bInitialized = false;
	};
};
