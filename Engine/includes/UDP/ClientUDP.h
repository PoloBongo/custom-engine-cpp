#pragma once

#include "TCP/Messages.h"
#include "TCP/Sockets.h"
#include "TCP/Errors.h"

#include <winSock2.h>
#include <vector>
#include <memory>


class ClientUDP
{
	friend class DistantClient;

public:
	ClientUDP();
	ClientUDP(const ClientUDP&) = delete;
	ClientUDP(ClientUDP&&) = delete;
	ClientUDP& operator=(const ClientUDP&) = delete;
	ClientUDP& operator=(ClientUDP&&) = delete;
	~ClientUDP();

	bool init(uint16_t port);
	void release();
	void sendTo(const sockaddr_storage& target, std::vector<uint8_t>&& data);
	void receive();
	std::vector<std::unique_ptr<Network::Messages::Base>> poll();

	void onMessageReady(std::unique_ptr<Network::Messages::Base>&& msg);
	DistantClient& getClient(const sockaddr_storage& clientAddr);

private:
	SOCKET mSocket{ INVALID_SOCKET };
	std::vector<std::unique_ptr<DistantClient>> mClients;
	std::vector<std::unique_ptr<Network::Messages::Base>> mMessages;
};

