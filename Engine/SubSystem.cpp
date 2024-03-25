#include "SubSystem.h"

// première option pour instancier le manager //
CollisionManager* gCollisionsManager; // global variable


// deuxième option pour instancier le manager //
std::unordered_map<std::type_index, void*> SubSystem::instances;

template<class T> void SubSystem::Set(T* instance) {
    instances[typeid(T)] = instance;
}

template<class T> T* SubSystem::Get() {
    auto it = instances.find(typeid(T));
    if (it != instances.end()) {
        return static_cast<T*>(it->second);
    }
    else {
        return nullptr;
    }
}

void startUp()
{
    // première option pour instancier le manager //
    gCollisionsManager = new CollisionManager;
    gCollisionsManager->start();

	// deuxième option pour instancier le manager //
	SubSystem::Set<CollisionManager>(&CollisionManager::get()); // ou SubSystem::Set<CollisionManager>(new CollisionManager());
	SubSystem::Get<CollisionManager>()->start();
}