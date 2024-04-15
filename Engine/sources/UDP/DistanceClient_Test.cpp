#include "UDP/DistanceClient_Test.h"
#include <UDP/DistantClient.h>

void DistanceClient_Test::Test()
{
	static constexpr uint64_t MASK_COMPLETE = std::numeric_limits<uint64_t>::max();
	static constexpr uint64_t MASK_FIRST_ACKED = Bousk::Utils::Bit<uint64_t>::Right;
	static constexpr uint64_t MASK_FIRST_AND_SECOND_ACKED = (MASK_FIRST_ACKED << 1) | Bousk::Utils::Bit<uint64_t>::Right;
	static constexpr uint64_t MASK_FIRST_MISSING = ~MASK_FIRST_ACKED;
	static constexpr uint64_t MASK_LAST_ACKED = (MASK_FIRST_ACKED << 63);

	ClientUDP client;
	sockaddr_in localAddress;
	localAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	localAddress.sin_family = AF_INET;
	localAddress.sin_port = htons(8888);
	DistantClient distantClient(client, reinterpret_cast<const sockaddr_storage&>(localAddress));

	CHECK(distantClient.mNextDatagramIdToSend == 0);
	CHECK(distantClient.mReceivedAcks.lastAck() == std::numeric_limits<uint16_t>::max());

	const char* TestString = "Test data";
	constexpr size_t TestStringLength = sizeof(TestString);
	distantClient.send(std::vector<uint8_t>(TestString, TestString + TestStringLength));
	CHECK(distantClient.mNextDatagramIdToSend == 1);

	//!< Créons un datagramme pour vérifier sa réception
	Bousk::UDP::Datagram datagram;
	datagram.header.id = 0;
	memcpy(datagram.data.data(), TestString, TestStringLength);
	datagram.datasize = TestStringLength;
	//!< Si un datagramme est accepté, il créera un Message UserData chez le Client
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 0);
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_COMPLETE);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Le datagramme #0 est reçu en double : il doit être ignoré
	datagram.header.id = 0;
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 0);
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_COMPLETE);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 0);
	}

	//!< Envoi du datagramme #2, le #1 est maintenant manquant
	datagram.header.id = htons(2);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 2);
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_FIRST_MISSING);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Réception du datagramme #1 désordonné
	datagram.header.id == htons(1);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 2);
		CHECK(distantClient.mReceivedAcks.isNewlyAcked(1));
		CHECK(!distantClient.mReceivedAcks.isNewlyAcked(2));
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_COMPLETE);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Saut de 64 datagrammes, tous les intermédiaires sont manquants
	datagram.header.id = htons(66);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 66);
		CHECK(distantClient.mReceivedAcks.isNewlyAcked(66));
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_LAST_ACKED);
		CHECK(distantClient.mReceivedAcks.loss().empty());

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Réception du datagramme suivant
	datagram.header.id = htons(67);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 67);
		CHECK(distantClient.mReceivedAcks.isNewlyAcked(67));
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_FIRST_ACKED);
		CHECK(distantClient.mReceivedAcks.loss().empty());

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Réception du suivant, le datagramme #3 est maintenant perdu
	datagram.header.id = htons(68);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 68);
		CHECK(distantClient.mReceivedAcks.isNewlyAcked(68));
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_FIRST_AND_SECOND_ACKED);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 1);
		const auto& msg = polledMessages[0];
		CHECK(msg->is<Network::Messages::UserData>());
		const auto dataMsg = msg->as<Network::Messages::UserData>();
		CHECK(dataMsg->data.size() == TestStringLength);
		CHECK(memcmp(TestString, dataMsg->data.data(), TestStringLength) == 0);
	}

	//!< Réception du datagramme #3 : trop ancien, ignoré
	datagram.header.id = htons(3);
	{
		distantClient.onDatagramReceived(std::move(datagram));
		CHECK(distantClient.mReceivedAcks.lastAck() == 68);
		CHECK(!distantClient.mReceivedAcks.isNewlyAcked(68));
		CHECK(distantClient.mReceivedAcks.previousAcksMask() == MASK_FIRST_AND_SECOND_ACKED);

		auto polledMessages = client.poll();
		CHECK(polledMessages.size() == 0);
	}
}