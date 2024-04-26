#pragma once
#include "TCP/Server/Server.h"
#include <thread>

class TCPServerStart
{
public:
	void TCPServer(unsigned short _port, std::string _ipAdress, bool _active = false);
	void serverThreadFunction(Network::TCP::Server& _server);
	
	const Network::TCP::Server& getServer() const {
		return server;
	}
private:
	Network::TCP::Server server;
	std::thread serverThread;
};