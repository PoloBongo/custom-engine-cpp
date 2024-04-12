#include "UDP/DistanceClient_Test.h"
#include <UDP/DistantClient.h>

void DistanceClient_Test::Test()
{
	//static constexpr uint64_t MASK_COMPLETE = std::numeric_limits<uint64_t>::max();
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
	//CHECK(distantClient.mReceivedAcks.lastAck() == std::numeric_limits<uint16_t>::max());

	const char* TestString = "Test data";
	constexpr size_t TestStringLenght = sizeof(TestString);
	distantClient.send(std::vector<uint8_t>(TestString, TestString + TestStringLenght));
	CHECK(distantClient.mNextDatagramIdToSend == 1);

	//!< Créons un datagramme pour vérifier sa réception
	Bousk::UDP::Datagram datagram;
	datagram.header.id = 0;
	memcpy(datagram.data.data(), TestString, TestStringLenght);
}