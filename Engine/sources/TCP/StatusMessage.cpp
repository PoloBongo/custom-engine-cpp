#include "TCP/StatusMessage.h"
#include <Windows.h>

std::string StatusMessage::getHostName()
{
    char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(hostname);

    if (GetComputerNameA(hostname, &size)) {
        return std::string(hostname);
    }
    else {
        return "Unknown";
    }
}