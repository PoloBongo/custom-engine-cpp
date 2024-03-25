#pragma once
#include <cassert>

class TextureManager
{
public:
	// la fonction static va être créer lors du première appel de la fonction
	static TextureManager& get() {
		static TextureManager* gpSingleton = nullptr;
		if (gpSingleton == nullptr) {
			gpSingleton = new TextureManager;
		}
		assert(gpSingleton);
		return *gpSingleton;
	}
private:
	TextureManager() {}

	~TextureManager() {}

	void Awake() {}

	void Create() {}

	void Update() {}

	void start() {}

	void stop() {}
};

