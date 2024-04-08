#pragma once

#include <vector>
#include "Modules/Module.h"

/**
 * @brief Classe ModuleManager.
 *
 * Cette classe représente le gestionnaire de modules dans le système.
 * Le ModuleManager est responsable de la création, de la gestion et de la communication entre les différents modules de l'application.
 */
class ModuleManager
{
	public:
		ModuleManager() = default;
		~ModuleManager();

		void CreateDefaultModules();

		void Init() const;
		void Start() const;
		void FixedUpdate() const;
		void Update() const;
		void PreRender() const;
		void Render() const;
		void RenderGui() const;
		void PostRender() const;
		void Release() const;
		void Finalize() const;

		template <class T>
		T* CreateModule();

		template <class T>
		T* GetModule();

	private:
		std::vector<Module*> modules;
};

template <class T>
T* ModuleManager::CreateModule()
{
	T* module            = new T();
	module->moduleManager = this;
	modules.push_back(module);
	return module;
}

template <class T>
T* ModuleManager::GetModule()
{
	for (Module*& module : modules)
	{
		if (T* module_cast = dynamic_cast<T*>(module)) return module_cast;
	}
	return nullptr;
}
