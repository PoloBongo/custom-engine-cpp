#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <vector>

class FilesDirs
{
	public:
		int FilesInDir(std::string _filepath);
		std::vector<std::wstring> GetFilesInDir(std::wstring _filepath);
		std::wstring ConvertStringToWideString(const std::string& narrow_string);
		std::string ConvertWideStringToString(const std::wstring& wide_string);
};

