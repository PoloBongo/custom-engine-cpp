#include "TCP/Client/TCPClientStart.h"
#include <thread>

void TCPClientStart::clientThreadFunction(Network::TCP::Client& client, bool messageSend, std::string data) {	
	while (true)
	{
		while (auto msg = client.poll())
		{
			if (msg->is<Network::Messages::Connection>())
			{
				auto connection = msg->as<Network::Messages::Connection>();
				if (connection->result == Network::Messages::Connection::Result::Success)
				{
					//std::cin.ignore();
					std::cout << "Connecte!" << std::endl;
					if (messageSend)
					{
						std::string phrase = data;
						
						//std::getline(std::cin, phrase);
						if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
						{
							std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
						}
					}
				}
				else
				{
					std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl;
				}
			}
			else if (msg->is<Network::Messages::UserData>())
			{
				auto userdata = msg->as<Network::Messages::UserData>();
				std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				std::cout << "Reponse du serveur : " << reply << std::endl;
				std::cout << ">";
				std::string phrase = "hello";
				//std::getline(std::cin, phrase);
				if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
				{
					std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
				}
			}
			else if (msg->is<Network::Messages::Disconnection>())
			{
				auto disconnection = msg->as<Network::Messages::Disconnection>();
				std::cout << "Deconnecte : " << static_cast<int>(disconnection->reason) << std::endl;
			}
		}
	}
}

void TCPClientStart::TCPClient(std::string ipAdress, int _port, bool messageSend, std::string data)
{
	if (!Network::Start())
	{
		std::cout << "Erreur initialisation WinSock : " << Network::Errors::Get() << std::endl;
	}
	else { std::cout << "initialisation WinSock" << std::endl; }

	if (!client.connect(ipAdress, _port))
	{
		std::cout << "Impossible de se connecter au serveur [" << "ipAdress: " << ipAdress << ":" << _port << "] : " << Network::Errors::Get() << std::endl;
	}
	else {
		std::thread clientThread(&TCPClientStart::clientThreadFunction, this, std::ref(client), std::ref(messageSend), std::ref(data));
		//clientThread.join();
	}
}