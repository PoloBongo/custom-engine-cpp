#include "Camera/BaseCamera.h"

namespace lve {
	BaseCamera::BaseCamera(const std::string& cameraName, CameraType type, bool bIsGameplayCam, float FOV, float zNear, float zFar) :
		bIsGameplayCam(bIsGameplayCam),
		type(type),
		m_Name(cameraName),
		m_View(MAT4_ZERO),
		m_Proj(MAT4_ZERO),
		m_ViewProjection(MAT4_ZERO),
		FOV(FOV),
		zNear(zNear),
		zFar(zFar),
		moveSpeed(18.0f),
		panSpeed(60.0f),
		dragDollySpeed(25.0f),
		scrollDollySpeed(10.0f),
		orbitingSpeed(0.03f),
		mouseRotationSpeed(0.015f),
		gamepadRotationSpeed(2.0f),
		moveSpeedFastMultiplier(3.0f),
		moveSpeedSlowMultiplier(0.08f),
		turnSpeedFastMultiplier(2.0f),
		turnSpeedSlowMultiplier(0.1f),
		panSpeedFastMultiplier(2.5f),
		panSpeedSlowMultiplier(0.2f),
		rollRestorationSpeed(12.0f),
		position(VEC3_ZERO),
		yaw(0.0f),
		pitch(0.0f),
		roll(0.0f),
		forward(VEC3_FORWARD),
		up(VEC3_UP),
		right(VEC3_RIGHT)
	{
		ResetOrientation();
		CalculateAxisVectorsFromPitchAndYaw();
		RecalculateViewProjection();
		CalculateExposure();
	}


	BaseCamera::~BaseCamera()
	{
	}

	void BaseCamera::Initialize()
	{
		m_bInitialized = true;
	}

	void BaseCamera::FixedUpdate()
	{
	}

	void BaseCamera::Update()
	{
		prevPosition = position;

		/*roll = Lerp(roll, 0.0f, rollRestorationSpeed * g_DeltaTime);

		AudioManager::SetListenerPos(position);
		AudioManager::SetListenerVel(velocity);*/
	}

	void BaseCamera::LateUpdate()
	{
	/*	velocity = (position - prevPosition) / g_DeltaTime;*/

	}

	void BaseCamera::Destroy()
	{
		if (m_bInitialized)
		{
			m_bInitialized = false;
		}
	}
	glm::mat4 BaseCamera::GetViewProjection() const
	{
		return m_ViewProjection;
	}

	glm::mat4 BaseCamera::GetView() const
	{
		return m_View;
	}

	glm::mat4 BaseCamera::GetProjection() const
	{
		return m_Proj;
	}

	void BaseCamera::LookAt(glm::vec3 point, real speed)
	{
		speed = Saturate(speed);

		glm::vec3 targetForward = glm::normalize(point - position);
		forward = glm::normalize(Lerp(forward, targetForward, speed));

#if THOROUGH_CHECKS
		if (IsNanOrInf(forward))
		{
			PrintError("Forward vector was NaN or Inf!\n");
			forward = VEC3_FORWARD;
		}
#endif

		CalculateYawAndPitchFromForward();
		RecalculateViewProjection();
	}

	void BaseCamera::Translate(glm::vec3 translation)
	{
		position += translation;
	}

	void BaseCamera::SetViewDirection(real yawRad, real pitchRad)
	{
		yaw = yawRad;
		pitch = pitchRad;
		roll = 0.0f;
	}

	void BaseCamera::ResetPosition()
	{
		position = VEC3_ZERO;
	}

	void BaseCamera::ResetOrientation()
	{
		pitch = 0.0f;
		yaw = PI;
		roll = 0.0f;
	}

} // namespace lve