#include "TCP/Client/TCPClientStart.h"
#include <mutex>

void TCPClientStart::threadClientUDPConnexionCheck(Network::TCP::Client& client, std::string _data) {
	StatusMessage& statusMsg = StatusMessage::getInstance();
	while (true)
	{
		while (auto msg = client.poll())
		{
			if (msg->is<Network::Messages::Connection>())
			{
				auto connection = msg->as<Network::Messages::Connection>();
				if (connection->result == Network::Messages::Connection::Result::Success)
				{
					std::cout << "connexion success" << std::endl;
					if (statusMsg.getStatus() == StatusMessage::Status::Send)
					{
						sendData(_data);
					}
				} else { std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl; }
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

std::string TCPClientStart::sendData(std::string _data)
{
	StatusMessage& statusMsg = StatusMessage::getInstance();
	//_data = "bongo";
	//std::string phrase = _data;
	//std::getline(std::cin, phrase);
	//if (statusMsg.getStatus() == StatusMessage::Status::Send)
	//{
	//	if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
	//	{
	//		std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
	//	}
	//	statusMsg.setStatus(StatusMessage::Status::None);
	//}

	if (!client.send(reinterpret_cast<const unsigned char*>(_data.c_str()), static_cast<unsigned int>(_data.length())))
	{
		std::cout << "Erreur envoi2 : " << Network::Errors::Get() << std::endl;
	}
	else { statusMsg.setStatus(StatusMessage::None); }

	return _data;
}

void TCPClientStart::ConnexionClientUDP(std::string ipAdress, int _port)
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
		clientThreadConnexionCheck = std::thread(&TCPClientStart::threadClientUDPConnexionCheck, this, std::ref(client), "");
	}
}

//void TCPClientStart::threadClientUDPDataSendCheck(Network::TCP::Client& _client)
//{
//	while (true)
//	{
//		while (auto msg = client.poll())
//		{
//			auto connection = msg->as<Network::Messages::Connection>();
//			if (connection->result == Network::Messages::Connection::Result::Success)
//			{
//				std::string phrase = "sendData(_data)";
//				if (!client.send(reinterpret_cast<const unsigned char*>(phrase.c_str()), static_cast<unsigned int>(phrase.length())))
//				{
//					std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
//				}
//				if (connection->result != Network::Messages::Connection::Result::Success) {
//					std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl;
//				}
//			}
//			else
//			{
//				std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl;
//			}
//		}
//	}
//}
//
//void TCPClientStart::SendDataClientUDP(std::string _data)
//{
//	clientThreadDataSendCheck = std::thread(&TCPClientStart::threadClientUDPDataSendCheck, this, std::ref(client));
//}