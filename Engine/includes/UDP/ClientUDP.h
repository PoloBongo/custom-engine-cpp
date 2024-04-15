#pragma once

#include "Messages.h"
#include "TCP/Sockets.h"
#include "Errors.h"

#include <winSock2.h>
#include <vector>
#include <memory>
#include <unordered_set>


namespace Bousk
{
	namespace Network
	{
		namespace Messages
		{
			class Base;
		}
		namespace UDP
		{
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
				Bousk::Network::UDP::DistantClient& getClient(const sockaddr_storage& clientAddr);

				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					inline void enableNetworkInterruption() { setNetworkInterruptionEnabled(true); }
					inline void disableNetworkInterruption() { setNetworkInterruptionEnabled(false); }
					inline void setNetworkInterruptionEnabled(bool enabled) { mNetworkInterruptionAllowed = enabled; }
					inline bool isNetworkInterruptionAllowed() const { return mNetworkInterruptionAllowed; }
					inline bool isNetworkInterrupted() const { return !mInterruptedClients.empty(); }
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

			private:
				SOCKET mSocket{ INVALID_SOCKET };
				std::vector<std::unique_ptr<Bousk::Network::UDP::DistantClient>> mClients;
				std::vector<std::unique_ptr<Network::Messages::Base>> mMessages;

				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					bool mNetworkInterruptionAllowed{ true };
					void onClientInterrupted(const DistantClient* client);
					void onClientResumed(const DistantClient* client);
					// Returns whether this client is the sole responsible for the network interruption
					bool isInterruptionCulprit(const DistantClient* client) const;
					std::unordered_set<const DistantClient*> mInterruptedClients;
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
			};
		}
	}
}

