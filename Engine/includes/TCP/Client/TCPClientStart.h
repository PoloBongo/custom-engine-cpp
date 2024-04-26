#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"

#include <iostream>
#include <thread>

class TCPClientStart
{
public:
	void TCPClient(std::string _ipAdress, int _port, bool _messageSend = false, std::string _data = "");
	void clientThreadFunction(Network::TCP::Client& _client, bool _messageSend = false, std::string _data = "");

	const Network::TCP::Client& getClient() const {
		return client;
	}

private:
	Network::TCP::Client client;
	std::thread clientThread;
};