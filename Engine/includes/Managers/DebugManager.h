#pragma once
#include <iostream>

class DebugManager
{
public:
    enum class TypeLog {
        INFO,
        WARNING,
        ERROR,
        SUCCESS
    };

    static void Log(const std::string& message, TypeLog level = TypeLog::INFO)
    {
        std::string prefixType;
        std::string colorCode;
        switch (level)
        {
        case TypeLog::INFO:
            prefixType = "[INFO]";
            colorCode = "\033[1;34m"; // Bleu
            break;
        case TypeLog::WARNING:
            prefixType = "[WARNING]";
            colorCode = "\033[1;33m"; // Jaune
            break;
        case TypeLog::ERROR:
            prefixType = "[ERROR]";
            colorCode = "\033[1;31m"; // Rouge
            break;
        case TypeLog::SUCCESS:
            prefixType = "[SUCCESS]";
            colorCode = "\033[1;32m"; // Vert
            break;
        default:
            break;
        }

        std::string resetColor = "\033[0m"; // Reset la couleur
        std::cout << colorCode << prefixType << " : " << message << resetColor << std::endl;
    }

    static void LogError(const std::string& message)
    {
        Log(message, TypeLog::ERROR);
    }

    static void LogWarning(const std::string& message)
    {
        Log(message, TypeLog::WARNING);
    }

    static void LogInfo(const std::string& message)
    {
        Log(message, TypeLog::INFO);
    }

    static void LogSuccess(const std::string& message)
    {
        Log(message, TypeLog::SUCCESS);
    }
};