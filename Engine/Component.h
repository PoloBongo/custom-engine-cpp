#pragma once
#include <string>
#include "lve_window.h"

class GameObject;

class Component
{
	public:
		Component() = default;
		explicit Component(GameObject* _owner);
		virtual  ~Component() = default;

		virtual void Start();
		virtual void Physics(const float& _delta);
		virtual void Update(const float& _delta);

		//Render Methods
		virtual void Render(lve::LveWindow* _window);
		virtual void RenderGui(lve::LveWindow* _window);
		virtual void RenderBackground(lve::LveWindow* _window);

		GameObject* GetOwner() const { return owner; }
		void        SetOwner(GameObject* _owner) { owner = _owner; }
		std::string GetName() const { return name; }
		void        SetName(const std::string& _name) { name = _name; }
		void        SetName(const char* _name) { name = _name; }

		void SetActive(const bool& _state) { isActive = _state; }
		bool GetActive() const { return isActive; }

		void SetVisible(const bool& _state) { isVisible = _state; }
		bool GetVisible() const { return isVisible; }

		void SetActiveAndVisible(const bool& _state)
		{
			isVisible = _state;
			isActive  = _state;
		}

	protected:
		GameObject* owner     = nullptr;
		std::string name      = "Component";
		bool        isActive  = true;
		bool        isVisible = true;
};
