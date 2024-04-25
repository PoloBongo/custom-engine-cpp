#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"

#include <iostream>

class TCPClientStart
{
public:
	void TCPClient(std::string ipAdress, int port, bool messageSend = false, std::string data = "");
	void clientThreadFunction(Network::TCP::Client& client, bool messageSend = false, std::string data = "");

	const Network::TCP::Client& getClient() const {
		return client;
	}

private:
	Network::TCP::Client client;
};