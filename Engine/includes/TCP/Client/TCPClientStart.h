#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"
#include "TCP/StatusMessage.h"
#include "TCP/Server/TCPServerStart.h"

#include <iostream>
#include <thread>

class TCPClientStart
{
public:
	TCPClientStart() : stopThread(false) {}
	~TCPClientStart() {
		stopClient();
	}

	void startClient();
	void stopClient();

	void ConnexionClientUDP(std::string _ipAdress, int _port);
	void threadClientUDPConnexionCheck(std::string _data = "");

	std::string sendData(std::string _data = "");

	const Network::TCP::Client& getClient() const {
		return client;
	}

	void setConnectedClient(bool isConnected);
	bool getConnectedClient() { return isConnected; }

private:
	Network::TCP::Client client;
	std::thread clientThreadConnexionCheck;
	bool isConnected = false;
	std::atomic<bool> stopThread;
};