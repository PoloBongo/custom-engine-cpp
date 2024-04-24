#include "TCP/Client/TCPClientStart.h"

int TCPClientStart::TCPClient()
{
	if (!Network::Start())
	{
		std::cout << "Error starting sockets : " << Network::Errors::Get() << std::endl;
		return -1;
	}

	Network::TCP::Client client;
	std::string ipAdress;
	std::cout << "Adresse IPv4 ? ";
	std::cin >> ipAdress;
	int port;
	std::cout << "Port du serveur ? ";
	std::cin >> port;
	if (!client.connect(ipAdress, port))
	{
		std::cout << "Impossible de se connecter au serveur [" << "ipAdress:" << port << "] : " << Network::Errors::Get() << std::endl;
	}
	else
	{
		while (1)
		{
			while (auto msg = client.poll())
			{
				if (msg->is<Network::Messages::Connection>())
				{
					auto connection = msg->as<Network::Messages::Connection>();
					if (connection->result == Network::Messages::Connection::Result::Success)
					{
						std::cin.ignore();
						std::cout << "Connecte!" << std::endl;
						std::cout << "Entrez une phrase >";
						std::string phrase;
						std::getline(std::cin, phrase);
						if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
						{
							std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
							break;
						}
					}
					else
					{
						std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl;
						break;
					}
				}
				else if (msg->is<Network::Messages::UserData>())
				{
					auto userdata = msg->as<Network::Messages::UserData>();
					std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
					std::cout << "Reponse du serveur : " << reply << std::endl;
					std::cout << ">";
					std::string phrase;
					std::getline(std::cin, phrase);
					if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
					{
						std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
						break;
					}
				}
				else if (msg->is<Network::Messages::Disconnection>())
				{
					auto disconnection = msg->as<Network::Messages::Disconnection>();
					std::cout << "Deconnecte : " << static_cast<int>(disconnection->reason) << std::endl;
					break;
				}
			}
		}
	}
	Network::Release();
	return 0;
}