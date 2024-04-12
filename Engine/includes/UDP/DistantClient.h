#pragma once

#include "Datagram.h"
#include "ClientUDP.h"
#include "AckHandler.h"
#include "Address.h"

#include "TCP/Messages.h"

#include <vector>
#include <memory> // unique_ptr

class DistantClient
{
public:
	DistantClient(ClientUDP& client, const Bousk::Network::Address& addr);
	DistantClient(const DistantClient&) = delete;
	DistantClient(DistantClient&&) = delete;
	DistantClient& operator=(const DistantClient&) = delete;
	DistantClient& operator=(DistantClient&&) = delete;
	~DistantClient() = default;

	void send(std::vector<uint8_t>&& data);
	void onDatagramReceived(Bousk::UDP::Datagram&& datagram);
	void onDataReceived(std::vector<uint8_t>&& data);
	void onMessageReady(std::unique_ptr<Network::Messages::Base>&& msg);

	void onDatagramSentAcked(Bousk::UDP::Datagram::ID datagramId);
	void onDatagramReceivedLost(Bousk::UDP::Datagram::ID datagramId);
	void onDatagramSentLost(Bousk::UDP::Datagram::ID datagramId);

	inline const Bousk::Network::Address& address() const { return mAddress; }
	
	Bousk::UDP::Datagram::ID mNextDatagramIdToSend{ 0 };
	Bousk::UDP::AckHandler mReceivedAcks;

private:
	SOCKET mSocket { INVALID_SOCKET };
	Bousk::Network::Address mAddress;
	Bousk::UDP::AckHandler mSentAcks;
	ClientUDP mClient;
};

