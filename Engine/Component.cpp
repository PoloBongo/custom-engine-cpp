#include "Component.h"
#include "Camera/BaseCamera.h"


Component::Component(GameObject* _owner)
{
	this->owner = _owner;
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

void Component::RenderGui(lve::LveWindow* _window)
{
	//_window->setView(HUDModule::GetHud());
}

void Component::RenderBackground(lve::LveWindow* _window)
{
	//_window->setView(BaseCamera::GetView());
}
