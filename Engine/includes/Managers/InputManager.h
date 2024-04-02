#pragma once
#include <iostream>
#include <unordered_set>

class InputManager
{
public:
	void Update();

	bool IsMouseButtonPressed(int _button);

	bool AnyKeyPressed(int _key);

	float GetMousePosition();

	bool IsMouseScrolledUp() {
		return scrollUp;
	}

	bool IsMouseScrolledDown() {
		return scrollDown;
	}

	bool IsKeyPressed(int _key) {
		return keysPressed.count(_key) > 0;
	}

	bool AreKeysPressed(std::initializer_list<int> _listKeys) {
		for (int key : _listKeys) {
			if (keysPressed.count(key) == 0) {
				return false;
			}
		}
		return true; 
	}

private:
	bool scrollUp;
	bool scrollDown;
	// unordered_set permet de stocker (dans notre cas) des int qui ne soit pas dupliquer
	std::unordered_set<int> keysPressed;
};

