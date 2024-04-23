#include "UDP/Network/ClientUDP/ClientUDPStart.h"
#include "UDP/SocketsUDP.h"
#include "UDP/ErrorsUDP.h"

#include <iostream>
#include <string>
#include <thread>

int ClientUDPStart::ClientStartUDP()
{
	if (!Bousk::Network::Start())
	{
		std::cout << "Network lib initialisation error : " << Bousk::Network::ErrorsUDP::Get();
		return -1;
	}

    SOCKET myFirstUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (myFirstUdpSocket == SOCKET_ERROR) {
        std::cout << "Socket creation error : " << Bousk::Network::ErrorsUDP::Get();
        return -2;
    }

    unsigned short portDst;
    std::cout << "Target port ? ";
    std::cin >> portDst;
    sockaddr_in to = { 0 };
    inet_pton(AF_INET, "127.0.0.1", &to.sin_addr.s_addr);
    to.sin_family = AF_INET;
    to.sin_port = htons(portDst);

    std::cout << "Envoie un message : ";
    while (true) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string data;
        std::getline(std::cin, data);
        //if (data.empty())
        //    break;
        int ret = sendto(myFirstUdpSocket, data.data(), static_cast<int>(data.length()), 0, reinterpret_cast<const sockaddr*>(&to), sizeof(to));
        if (ret <= 0) {
            std::cout << "Data send error : " << Bousk::Network::ErrorsUDP::Get() << ". Closing program.";
            break;
        }
    }

    Bousk::Network::CloseSocket(myFirstUdpSocket);
    Bousk::Network::Release();
    return 0;
}