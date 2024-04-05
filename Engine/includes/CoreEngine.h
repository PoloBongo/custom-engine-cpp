#pragma once

#include "ModuleManager.h"

class Engine
{
	public:
		static Engine* GetInstance();

		void Init() const;
		void Run() const;
		void Quit() { shouldQuit = true; }

		ModuleManager* GetModuleManager() const { return moduleModule; }

	private:
		static Engine* instance;

		ModuleManager* moduleModule = new ModuleManager;

		bool shouldQuit = false;
};
