#include "Component.h"
#include "Camera/BaseCamera.h"


Component::Component(GameObject* owner)
{
	this->owner = owner;
}

void Component::Start()
{
}

void Component::Physics(const float& _delta)
{
}

void Component::Update(const float& _delta)
{
}


void Component::Render(lve::LveWindow* _window)
{
	//_window->setView(lve::BaseCamera::GetView());
}

void Component::RenderGUI(lve::LveWindow* _window)
{
	//_window->setView(HUDModule::GetHud());
}

void Component::RenderBackground(lve::LveWindow* _window)
{
	//_window->setView(BaseCamera::GetView());
}
