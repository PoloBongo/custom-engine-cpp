#include "UDP/Network/ServerUDP/ServerUDPStart.h"
#include "UDP/SocketsUDP.h"
#include "UDP/ErrorsUDP.h"

#include <iostream>
#include <string>
#include <thread>

int ServerUDPStart::ServerStartUDP()
{
    if (!Bousk::Network::Start()) {
        std::cout << "Network lib initialisation error : " << Bousk::Network::ErrorsUDP::Get();
        return -1;
    }

    SOCKET myFirstUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (myFirstUdpSocket == SOCKET_ERROR) {
        std::cout << "Socket creation error : " << Bousk::Network::ErrorsUDP::Get();
        return -2;
    }

    unsigned short port;
    std::cout << "Ouvrir un port ? ";
    std::cin >> port;

    sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    if (bind(myFirstUdpSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        std::cout << "Socket bind error : " << Bousk::Network::ErrorsUDP::Get();
        return -3;
    }

    while (true) {
        char buff[1500] = { 0 };
        sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        int ret = recvfrom(myFirstUdpSocket, buff, 1499, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
        if (ret <= 0) {
            std::cout << "Data reception error : " << Bousk::Network::ErrorsUDP::Get() << ". Closing program.";
            break;
        }
        std::cout << "Received : " << buff << " from " << Bousk::Network::GetAddress(from) << ":" << Bousk::Network::GetPort(from) << std::endl;
    }

    Bousk::Network::CloseSocket(myFirstUdpSocket);
    Bousk::Network::Release();
    return 0;
}