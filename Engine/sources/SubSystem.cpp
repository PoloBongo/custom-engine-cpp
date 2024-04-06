#include "SubSystem.h"

// première option pour instancier le manager //


// deuxième option pour instancier le manager //
std::unordered_map<std::type_index, void*> SubSystem::instances;

template <class T>
void SubSystem::Set(T* _instance)
{
	instances[typeid(T)] = _instance;
}

template <class T>
T* SubSystem::Get()
{
	auto it = instances.find(typeid(T));
	if (it != instances.end()) return static_cast<T*>(it->second);
	return nullptr;
}

void startUp()
{
}
