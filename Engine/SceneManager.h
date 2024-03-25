#pragma once
#include <iostream>
#include <string>
#include <map>

class SceneManager
{
private:
	std::map<std::string, bool> listScenes;
	int sceneCount;
	bool sceneActive;
public:
	SceneManager() {}

	~SceneManager() {}

	void CreateScene(std::string _name, bool _isActive);

	void LoadScene(std::string _name);

	std::string GetActiveScene();

	int SceneCount();

};

