#include "UDP/SocketsUDP.h"
#include "UDP/ClientUDP.h"
#include "UDP/Protocol/ReliableOrdered.h"
#include "UDP/Serialization/Deserializer.h"
#include "UDP/Serialization/Serializer.h"
#include "UDP/Messages.h"
#include "UDP/ErrorsUDP.h"

#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int Connexion()
{
	if (Bousk::Network::Start())
	{
		std::cout << "Erreur d'init : " << Bousk::Network::ErrorsUDP::Get();
		return -1;
	}

	const Bousk::Network::Address client1 = Bousk::Network::Address::Loopback(Bousk::Network::Address::Type::IPv4, 8888);
	const Bousk::Network::Address client2 = Bousk::Network::Address::Loopback(Bousk::Network::Address::Type::IPv4, 9999);

	const std::vector<std::string> messagesToSend = { "mon", "premier", "transfert", "udp", "ordonne", "fiable" };
	std::mutex countMutex;
	// Créer un thread par client;
	std::thread t1([&]()
	{
		Bousk::Network::UDP::ClientUDP client;
		//client.registerChannel<Bousk::Network::UDP::Protocols::ReliableOrdered>();
	});
}