#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <vector>

class FilesDirs
{
	public:

		/**
		* @brief Return the numbers of files in a directory.
		*/
		int FilesInDir(std::string _filepath);

		int CountDirectories(const std::string& directory_path);

		std::vector<std::string> GetDirectoryNames(const std::string& directory_path);

		/**
		* @brief Return the list of the files in a directory.
		*/
		std::vector<std::wstring> GetFilesInDir(std::wstring _filepath);
		
		/**
		* @brief Convert the string to a wide string.
		*/
		std::wstring ConvertStringToWideString(const std::string& narrow_string);

		/**
		* @brief Convert the wide string to a string.
		*/
		std::string ConvertWideStringToString(const std::wstring& wide_string);

		bool createEngineDataDirectory();

		bool createANFileInDirectory();

		std::vector<std::wstring> readANFile();

		std::wstring openFileDialog();

		bool addPathToANFile(const std::wstring& newPath);

		bool pathExistsInANFile(const std::wstring& filePath);

		void ExtractFilenameAndExtension(const std::string& filePath, std::string& filename, std::string& extension);

		bool ContainsSubstring(const std::string& main_string, const std::string& substring);
		bool IsImageExtension(const std::string& ext);
};

