#include "TCP/Sockets.h"
#include "TCP/Messages.h"
#include "TCP/Server/TCPServerStart.h"

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
				server.sendToAll(userdata->data.data(), static_cast<unsigned int>(userdata->data.size()));
				//std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				//std::cout << "Reponse du client : " << reply << std::endl;
				//std::cout << ">";
				//TCPClientStart tcp;
				//std::string phrase = tcp.sendData();
			}
		}
		// Permet de mettre le thread en pause pour éviter la surcharge de la CPU
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
		//serverThread.join();
	}
}