#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"
#include "TCP/StatusMessage.h"

#include <iostream>
#include <thread>

class TCPClientStart
{
public:
	void ConnexionClientUDP(std::string _ipAdress, int _port);
	void SendDataClientUDP(std::string _data);
	void threadClientUDPConnexionCheck(Network::TCP::Client& _client, std::string _data = "");
	void threadClientUDPDataSendCheck(Network::TCP::Client& _client);

	std::string sendData(std::string _data = "");

	const Network::TCP::Client& getClient() const {
		return client;
	}

private:
	Network::TCP::Client client;
	std::thread clientThreadConnexionCheck;
	std::thread clientThreadDataSendCheck;
	StatusMessage* stats;
};