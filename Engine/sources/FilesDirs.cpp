#include "FilesDirs.h"
#include <unordered_set>


int FilesDirs::FilesInDir(std::string _filepath) 
{
    std::string directory_path = _filepath + "\\*";
    int file_count = 0;
    WIN32_FIND_DATAA file_data;
    HANDLE hFind = FindFirstFileA(directory_path.c_str(), &file_data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                ++file_count;
            }
        } while (FindNextFileA(hFind, &file_data) != 0);
        FindClose(hFind);
        //std::cout << "Number of files in directory: " << file_count << std::endl;
        return file_count;
    }
    else {
        //std::cerr << "Failed to open directory." << std::endl;
        return -1;
    }

    return 0;
}

std::vector<std::wstring> FilesDirs::GetFilesInDir(std::wstring _filepath)
{
    std::vector<std::wstring> file_names;
    std::wstring directory_path = _filepath + L"\\*"; 
    WIN32_FIND_DATAW file_data;
    HANDLE hFind = FindFirstFileW(directory_path.c_str(), &file_data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { 
                file_names.push_back(file_data.cFileName);
            }
        } while (FindNextFileW(hFind, &file_data) != 0);
        FindClose(hFind);
    }
    else {
        DWORD error = GetLastError();
    }

    return file_names;
}

std::wstring FilesDirs::ConvertStringToWideString(const std::string& narrow_string) {
    int wide_string_length = MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, nullptr, 0);
    wchar_t* wide_string_buffer = new wchar_t[wide_string_length];
    MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, wide_string_buffer, wide_string_length);
    std::wstring wide_string(wide_string_buffer);
    delete[] wide_string_buffer;

    return wide_string;
}

std::string FilesDirs::ConvertWideStringToString(const std::wstring& wide_string) {
    int narrow_string_length = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    char* narrow_string_buffer = new char[narrow_string_length];
    WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, narrow_string_buffer, narrow_string_length, nullptr, nullptr);
    std::string narrow_string(narrow_string_buffer);
    delete[] narrow_string_buffer;

    return narrow_string;
}

void FilesDirs::ExtractFilenameAndExtension(const std::string& filePath, std::string& filename, std::string& extension) {
    // Find the position of the last '/' or '\' character in the path
    size_t lastSlashPos = filePath.find_last_of("/\\");

    // Extract the filename from the path
    filename = (lastSlashPos != std::string::npos) ? filePath.substr(lastSlashPos + 1) : filePath;

    // Find the position of the last '.' character in the filename
    size_t lastDotPos = filename.find_last_of('.');

    // Extract the extension from the filename
    extension = (lastDotPos != std::string::npos) ? filename.substr(lastDotPos + 1) : "";
}

bool FilesDirs::ContainsSubstring(const std::string& main_string, const std::string& substring) {
    return main_string.find(substring) != std::string::npos;
}

bool FilesDirs::IsImageExtension(const std::string& ext) {
    static const std::unordered_set<std::string> imageExtensions = { "png", "jpg", "gif", "tga", "bmp", "psd", "hdr", "pic" };
    return imageExtensions.count(ext) > 0;
}