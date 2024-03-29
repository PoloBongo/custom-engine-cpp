#pragma once

// lve
#include "lve_constants.h"
#include "functionHelpers.h"

// Librairies
#include <glm.hpp>

// std
#include <string>

namespace lve {
	enum class CameraType
	{
		DEBUG_CAM,
		OVERHEAD,
		FIRST_PERSON,
		TERMINAL,
		VEHICLE,

		_NONE
	};

	class BaseCamera
	{
		BaseCamera(const std::string& cameraName, CameraType type, bool bIsGameplayCam, float FOV = glm::radians(45.0f),
			float zNear = 0.5f, float zFar = 1000.0f);
		virtual ~BaseCamera();

		virtual void Initialize();
		virtual void FixedUpdate();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Destroy();

		glm::mat4 GetViewProjection() const;
		glm::mat4 GetView() const;
		glm::mat4 GetProjection() const;

		// speed: Lerp amount to new rotation
		void LookAt(glm::vec3 point, float speed = 1.0f);

		void Translate(glm::vec3 translation);

		void SetViewDirection(float yawRad, float pitchRad);

		void ResetPosition();
		void ResetOrientation();

		std::string GetName() const;

		void CalculateExposure();


		// Exposure control
		float aperture = 1.0f; // f-stops
		float shutterSpeed = 1.0f / 8.0f; // seconds
		float lightSensitivity = 800.0f; // ISO
		float exposure = 0.0f;

		float FOV = 0.0f;
		float zNear = 0.0f;
		float zFar = 0.0f;

		float moveSpeed = 0.0f;				// WASD or gamepad left stick
		float panSpeed = 0.0f;				// MMB
		float dragDollySpeed = 0.0f;			// RMB
		float scrollDollySpeed = 0.0f;		// Scroll wheel
		float orbitingSpeed = 0.0f;			// Alt-LMB drag
		float mouseRotationSpeed = 0.0f;		// LMB drag
		float gamepadRotationSpeed = 0.0f;	// Gamepad right stick
		float moveSpeedFastMultiplier = 0.0f;
		float moveSpeedSlowMultiplier = 0.0f;
		float turnSpeedFastMultiplier = 0.0f;
		float turnSpeedSlowMultiplier = 0.0f;
		float panSpeedFastMultiplier = 0.0f;
		float panSpeedSlowMultiplier = 0.0f;
		float rollRestorationSpeed = 0.0f;

		glm::vec3 position;
		glm::vec3 prevPosition;
		glm::vec3 velocity;

		float yaw;
		float pitch;
		float roll;
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;

		bool bIsGameplayCam = true;
		bool bIsFirstPerson = false;
		bool bDEBUGCyclable = true;
		bool bPossessPlayer = false;

		CameraType type;

	protected:
		// Sets right, up, and forward based on yaw and pitch
		void CalculateAxisVectorsFromPitchAndYaw();
		void CalculateYawAndPitchFromForward();
		void RecalculateViewProjection();

		void JitterMatrix(glm::mat4& matrix);
		void ClampPitch();

		// Exposure calculations taken from Google's Filament rendering engine
		// Computes the camera's EV100
		// aperture measured in f-stops
		// shutterSpeed measured in seconds
		// sensitivity measured in ISO
		static float CalculateEV100(float aperture, float shutterSpeed, float sensitivity);

		// Computes the exposure normalization factor from the camera's EV100
		static float ComputeExposureNormFactor(float EV100);

		bool m_bInitialized = false;

		std::string m_Name;

		glm::mat4 m_View;
		glm::mat4 m_Proj;
		glm::mat4 m_ViewProjection;
	};
} // namespace lve
