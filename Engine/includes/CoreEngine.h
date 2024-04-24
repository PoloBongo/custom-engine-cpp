#pragma once

#include "ModuleManager.h"

enum EngineMode
{
	Editor,
	Play,
	Pause
};

class Engine
{
	public:
		static Engine* GetInstance();

		void Init() const;
		void Run() const;
		void Quit() { shouldQuit = true; }

		[[nodiscard]] ModuleManager* GetModuleManager() const { return moduleManager; }
		[[nodiscard]] EngineMode GetEngineMode() const { return engineMode; }

		void SetEngineMode(const EngineMode _engineMode) { engineMode = _engineMode; }
		void PlayEngineMode() { engineMode = Play; }
		void PauseEngineMode() { engineMode = Pause; }
		void EditorEngineMode() { engineMode = Editor; }
		

	private:
		static Engine* instance;

		ModuleManager* moduleManager = new ModuleManager;

		bool shouldQuit = false;

		EngineMode engineMode = Editor;
};
