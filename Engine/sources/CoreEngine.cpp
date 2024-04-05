#include "CoreEngine.h"

//#include <imgui.h>
//#include <SFML/Graphics/RenderWindow.hpp>

Engine* Engine::instance = nullptr;

Engine* Engine::GetInstance()
{
	if (instance == nullptr) instance = new Engine();

	return instance;
}

void Engine::Init() const
{
	moduleModule->CreateDefaultModules();
	moduleModule->Init();
}

void Engine::Run() const
{
	moduleModule->Start();

	while (!shouldQuit)
	{
		moduleModule->Update();
		moduleModule->PreRender();
		moduleModule->Render();
		moduleModule->RenderGui();
		moduleModule->PostRender();
	}

	moduleModule->Release();
	moduleModule->Finalize();
}
