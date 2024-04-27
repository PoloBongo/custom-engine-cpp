#pragma once
#include "TCP/Server/Server.h"
#include "TCP/StatusMessage.h"

#include <thread>

class TCPServerStart
{
public:
	void TCPServer(unsigned short _port, std::string _ipAdress, bool _active = false);
	void serverThreadFunction(Network::TCP::Server& _server);

	static TCPServerStart& getInstance() {
		static TCPServerStart instance; // Crée une seule instance de StatusMessage
		return instance;
	}

	void setData(const std::string& newData);
	std::string getData() const {
		return stockData;
	}
private:
	Network::TCP::Server server;
	std::thread serverThread;
	std::string stockData;
};