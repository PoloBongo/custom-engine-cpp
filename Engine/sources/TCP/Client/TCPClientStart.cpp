#include "TCP/Client/TCPClientStart.h"
#include <mutex>

void TCPClientStart::threadClientUDPConnexionCheck(std::string _data) {
	StatusMessage& statusMsg = StatusMessage::getInstance();
	while (!getConnectedClient())
	{
		while (auto msg = client.poll())
		{
			if (msg->is<Network::Messages::Connection>())
			{
				auto connection = msg->as<Network::Messages::Connection>();
				if (connection->result == Network::Messages::Connection::Result::Success)
				{
					std::cout << "connexion success" << std::endl;
				} else { std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl; }
			}
			else if (msg->is<Network::Messages::UserData>())
			{
				auto userdata = msg->as<Network::Messages::UserData>();
				std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				std::cout << "Reponse du serveur : " << reply << "Pseudo Client : " << statusMsg.getPseudo() << std::endl;
				std::cout << ">";
				if (statusMsg.getStatus() == StatusMessage::Status::Send)
				{
					sendData(_data);
				}
				statusMsg.addMessage(reply, statusMsg.getPseudo());
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

	if (!client.send(reinterpret_cast<const unsigned char*>(_data.c_str()), static_cast<unsigned int>(_data.length())))
	{
		std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
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
		startClient();
	}
}

void TCPClientStart::setConnectedClient(bool connected) {
	client.disconnect();
	Network::Release();
	isConnected = connected;
	stopClient();
}

void TCPClientStart::stopClient() {
	stopThread = true;
	if (clientThreadConnexionCheck.joinable()) {
		clientThreadConnexionCheck.join();
	}
}

void TCPClientStart::startClient() {
	clientThreadConnexionCheck = std::thread(&TCPClientStart::threadClientUDPConnexionCheck, this, "");
}