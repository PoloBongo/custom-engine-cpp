#pragma once
class TextureManager
{
public:
	static TextureManager& get()
	{
		// la fonction static va être créer lors du première appel de la fonction
		static TextureManager sSingleton;
		return sSingleton;
	}
private:
	TextureManager() {}

	~TextureManager() {}

	void start() {}

	void stop() {}
};

