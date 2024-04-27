#include "TCP/Sockets.h"
#include "TCP/Messages.h"
#include "TCP/Server/TCPServerStart.h"
#include "TCP/Client/TCPClientStart.h"

#include "TCP/Errors.h"

#include <iostream>

void TCPServerStart::serverThreadFunction(Network::TCP::Server& server) {
	while (true)
	{
		server.update();
		while (auto msg = server.poll())
		{
			if (msg->is<Network::Messages::Connection>()) {
				std::cout << "Connexion de [" << Network::GetAddress(msg->from) << ":" << Network::GetPort(msg->from) << "]" << std::endl;
			}
			else if (msg->is<Network::Messages::Disconnection>()) {
				std::cout << "Deconnexion de [" << Network::GetAddress(msg->from) << ":" << Network::GetPort(msg->from) << "]" << std::endl;
			}
			else if (msg->is<Network::Messages::UserData>()) {
				auto userdata = msg->as<Network::Messages::UserData>();
				StatusMessage& statusMsg = StatusMessage::getInstance();
				const auto& clients = server.getClients();

				std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				statusMsg.addMessage(reply, statusMsg.getPseudo());
				std::cout << "Pseudo Server : " << statusMsg.getPseudo() << std::endl;
				for (const auto& pair : clients) {
					uint64_t clientId = pair.first;
					const Network::TCP::Client& client = pair.second;

					const sockaddr_in& address = client.destinationAddress();
					std::string ipAddress = Network::GetAddress(address);
					unsigned short port = Network::GetPort(address);
					server.sendTo(clientId, userdata->data.data(), reply.size());

					std::cout << "Client ID: " << clientId << ", IP Address: " << ipAddress << ", Port: " << port << " Message :" << reply << std::endl;
				}
			}
		}
	}
	server.stop();
	Network::Release();
}

void TCPServerStart::TCPServer(unsigned short _port, std::string ipAdress, bool active)
{
	std::cout << "Active le port : " << _port << std::endl;
	if (!Network::Start())
	{
		std::cout << "Erreur initialisation WinSock : " << Network::Errors::Get() << std::endl;
	}
	else { std::cout << "initialisation WinSock" << std::endl; }

	if (!server.start(_port))
	{
		std::cout << "Erreur initialisation serveur : " << Network::Errors::Get() << std::endl;
	}
	else {
		std::cout << "initialisation du serveur" << std::endl;
		// Démarre le thread
		serverThread = std::thread(&TCPServerStart::serverThreadFunction, this, std::ref(server));
	}
}

void TCPServerStart::setData(const std::string& newData) {
	stockData = newData;
}