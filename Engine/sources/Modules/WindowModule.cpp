#include "Modules/WindowModule.h"
#include "CoreEngine.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <numeric>
#include <stdexcept>

#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"

void WindowModule::Init()
{
	Module::Init();

	lveWindow = new lve::LveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; /// Fenêtre de l'application.
}

void WindowModule::Start()
{
	Module::Start();
}

void WindowModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void WindowModule::Update()
{
	Module::Update();

	if (!lveWindow->ShouldClose())
	{
		glfwPollEvents();
	}
	else
	{
		moduleManager->GetModule<RHIModule>()->GetDevice()->Device().waitIdle();
		Engine::GetInstance()->Quit();
	}
}

void WindowModule::PreRender()
{
	Module::PreRender();
}

void WindowModule::Render()
{
	Module::Render();
	
}

void WindowModule::RenderGui()
{
	Module::RenderGui();
}

void WindowModule::PostRender()
{
	Module::PostRender();

	//window->display();
}

void WindowModule::Release()
{
	Module::Release();

	//window->close();
}
void WindowModule::Finalize()
{
	Module::Finalize();

	//window->close();
}


//void WindowModule::PreRender()
//{
//	Module::PreRender();
//
//	//window->clear(sf::Color::Black);
//}
//
//void WindowModule::Render()
//{
//	Module::Render();
//}
//
//void WindowModule::RenderGui()
//{
//	Module::RenderGui();
//}
//
//void WindowModule::PostRender()
//{
//	Module::PostRender();
//
//	//window->display();
//}
//
//void WindowModule::Release()
//{
//	Module::Release();
//
//	//window->close();
//}
