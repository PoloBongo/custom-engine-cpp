#include "UDP/ClientUDP.h"
#include <UDP/DistantClient.h>


ClientUDP::ClientUDP()
{
}

ClientUDP::~ClientUDP()
{
}

bool ClientUDP::init(uint16_t port)
{
	release();
	mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocket == INVALID_SOCKET)
		return false;

	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	int res = bind(mSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
	if (res != 0)
		return false;
	if (!Network::SetNonBlocking(mSocket))
		return false;

	return true;
}

void ClientUDP::release()
{
}

void ClientUDP::onMessageReady(std::unique_ptr<Network::Messages::Base>&& msg)
{
	mMessages.push_back(std::move(msg));
}

DistantClient& ClientUDP::getClient(const sockaddr_storage& clientAddr)
{
	auto itClient = std::find_if(
		mClients.begin(), mClients.end(), [&](const std::unique_ptr<DistantClient>& client)
		{
			return memcmp(&(client->address()), &clientAddr, sizeof(sockaddr_storage)) == 0; });
	if (itClient != mClients.end())
		return *(itClient->get());

	mClients.emplace_back(std::make_unique<DistantClient>(*this, clientAddr));
	return *(mClients.back());
}

void ClientUDP::sendTo(const sockaddr_storage& target, std::vector<uint8_t>&& data)
{
	auto& client = getClient(target);
	client.send(std::move(data));
}

void ClientUDP::receive()
{
	for (;;)
	{
		Bousk::UDP::Datagram datagram;
		sockaddr_in from{ 0 };
		socklen_t fromlen = sizeof(from);
		int ret = recvfrom(mSocket, reinterpret_cast<char*>(&datagram), Bousk::UDP::Datagram::BufferMaxSize, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
		if (ret > 0)
		{
			if (ret > Bousk::UDP::Datagram::HeaderSize)
			{
				datagram.datasize = ret - Bousk::UDP::Datagram::HeaderSize;
				auto& client = getClient(reinterpret_cast<sockaddr_storage&>(from));
				client.onDatagramReceived(std::move(datagram));
			}
			else
			{
				//!< Datagramme innatendu
			}
		}
		else
		{
			if (ret < 0)
			{
				//!< Gestion des erreurs
				const auto err = Network::Errors::Get();
				if (err != Network::Errors::WOULDBLOCK)
				{
					//!< Une erreur s’est produite
				}
			}
			return;
		}
	}
}

std::vector<std::unique_ptr<Network::Messages::Base>> ClientUDP::poll()
{
	return std::vector<std::unique_ptr<Network::Messages::Base>>();
}
