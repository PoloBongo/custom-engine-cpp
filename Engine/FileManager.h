#pragma once
#include <fstream>

class FileManager
{
	public:
	static bool FileExists(const std::string& _filePath)
	{
		const std::ifstream file(_filePath);
		return file.good();
	}
};

