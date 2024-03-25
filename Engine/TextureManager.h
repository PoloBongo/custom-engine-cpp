#pragma once
#include <cassert>
#include <iostream>

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

	TextureManager() {};

	TextureManager(const std::string& _filePath) : filePath(_filePath) {
		std::cout << "Chargement de la texture depuis : " << filePath << std::endl;
	}

	~TextureManager() {}

	void Awake() {}

	void Create() {}

	void Update() {}

	void Start() {}

	void Stop() {}

private:
	std::string filePath;
};

