#pragma once

#include "Address.h"
#include "DistantClient.h"
#include "SocketsUDP.h"
#include "Protocol/Simulator.h"
#include "Types.h"

#include <winSock2.h>
#include <vector>
#include <memory>
#include <unordered_set>
#include <mutex>
#include <functional>

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

				template<class T>
				void registerChannel(uint8 channelId = 0);

				#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
					Simulator& simulator() { return mSimulator; }
					const Simulator& simulator() const { return mSimulator; }
				#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED

				bool init(uint16_t port);
				void release();

				static void SetTimeout(std::chrono::milliseconds timeout);
				static std::chrono::milliseconds GetTimeout();

				void connect(const Address& addr);
				void disconnect(const Address& addr);
				void sendTo(const Address& target, std::vector<uint8>&& data, uint32 channelIndex);
				void sendTo(const Address& target, const uint8* data, size_t dataSize, uint32 channelIndex) { sendTo(target, std::vector<uint8>(data, data + dataSize), channelIndex); }
				void processSend();
				void receive();
				std::vector<std::unique_ptr<Messages::Base>> poll();

				void onMessageReady(std::unique_ptr<Messages::Base>&& msg);
				DistantClient* getClient(const Address& clientAddr, bool create = false);
				void setupChannels(DistantClient& client);

				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					inline void enableNetworkInterruption() { setNetworkInterruptionEnabled(true); }
					inline void disableNetworkInterruption() { setNetworkInterruptionEnabled(false); }
					inline void setNetworkInterruptionEnabled(bool enabled) { mNetworkInterruptionAllowed = enabled; }
					inline bool isNetworkInterruptionAllowed() const { return mNetworkInterruptionAllowed; }
					inline bool isNetworkInterrupted() const { return !mInterruptedClients.empty(); }
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

			private:
				SOCKET mSocket{ INVALID_SOCKET };
				std::vector<std::unique_ptr<DistantClient>> mClients;
				std::vector<std::unique_ptr<Messages::Base>> mMessages;
				uint64 mClientIdsGenerator{ 0 };
				std::mutex mMessagesLock;
				using MessagesLock = std::lock_guard<decltype(mMessagesLock)>;

				struct ChannelRegistration
				{
					std::function<void(DistantClient&)> creator;
					uint8 channelId;
				};
				std::vector<ChannelRegistration> mRegisteredChannels;

				struct Operation
				{
				public:
					enum class Type
					{
						Connect,
						SendTo,
						Disconnect,
					};
				public:
					static Operation Connect(const Address& target) { return Operation(Type::Connect, target); }
					static Operation SendTo(const Address& target, std::vector<uint8>&& data, uint32 channel) { return Operation(Type::SendTo, target, std::move(data), channel); }
					static Operation Disconnect(const Address& target) { return Operation(Type::Disconnect, target); }

					Operation(Type type, const Address& target) : mType(type), mTarget(target) {}
					Operation(Type type, const Address& target, std::vector<uint8>&& data, uint32 channel) : mType(type), mTarget(target), mData(std::move(data)), mChannel(channel) {}
					
					Type mType;
					Address mTarget;
					std::vector<uint8> mData;
					uint32 mChannel{ 0 };
				};
				std::mutex mOperationsLock;
				using OperationsLock = std::lock_guard<decltype(mOperationsLock)>;
				std::vector<Operation> mPendingOperations;

				#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
					Simulator mSimulator;
				#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					bool mNetworkInterruptionAllowed{ true };
					void onClientInterrupted(const DistantClient* client);
					void onClientResumed(const DistantClient* client);
					// Returns whether this client is the sole responsible for the network interruption
					bool isInterruptionCulprit(const DistantClient* client) const;
					std::unordered_set<const DistantClient*> mInterruptedClients;
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
			};

			template<class T>
			void ClientUDP::registerChannel(uint8 channelId /*= 0*/)
			{
				assert(mSocket == INVALID_SOCKET); // Don't add channels after being initialized !!!
				assert(std::find_if(mRegisteredChannels.begin(), mRegisteredChannels.end(), [&](const ChannelRegistration& registration) { return registration.channelId == channelId; }) == mRegisteredChannels.end());
				mRegisteredChannels.push_back({ [channelId](DistantClient& distantClient) { distantClient.registerChannel<T>(channelId); }, channelId });
			}
		}
	}
}

