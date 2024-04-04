#pragma once
#include <string>
#include "lve_window.h"

class GameObject;

class Component
{
public:
	Component() = default;
	explicit Component(GameObject* owner);
	virtual ~Component() = default;

	virtual void Start();
	virtual void Physics(const float& _delta);
	virtual void Update(const float& _delta);

	//Render Methods
	virtual void Render(lve::LveWindow* _window);
	virtual void RenderGUI(lve::LveWindow* _window);
	virtual void RenderBackground(lve::LveWindow* _window);

	inline GameObject* GetOwner() const { return owner; }
	inline void SetOwner(GameObject* _owner) { owner = _owner; }
	inline std::string GetName() const { return name; }
	inline void SetName(const std::string& _name) { name = _name; }
	inline void SetName(const char* _name) { name = _name; }

	inline void SetActive(const bool& _state) { isActive = _state; }
	inline bool GetActive() { return isActive; }

	inline void SetVisible(const bool& _state) { isVisible = _state; }
	inline bool GetVisible() { return isVisible; }

	inline void SetActiveAndVisible(const bool& _state)
	{
		isVisible = _state;
		isActive = _state;
	}

protected:
	GameObject* owner = nullptr;
	std::string name = "Component";
	bool isActive = true;
	bool isVisible = true;
};

