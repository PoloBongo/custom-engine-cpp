#include "UDP/ClientUDP.h"
#include "UDP/DistantClient.h"
#include "UDP/Sockets.h"

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			ClientUDP::ClientUDP()
			{
			}

			ClientUDP::~ClientUDP()
			{
			}

			// permet de créer un socket qui sera associé au port spécifié par l'utilisateur
			// le socket est config en mode non bloquant ( "SetNonBlocking()" )
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
				if (!SetNonBlocking(mSocket))
					return false;

				return true;
			}

			void ClientUDP::release()
			{
			}

			// ajoute le messages qui est prêt à être traité à la file d'attente
			void ClientUDP::onMessageReady(std::unique_ptr<Network::Messages::Base>&& msg)
			{
				mMessages.push_back(std::move(msg));
			}

			// permet d'obtenir le client qui est associé à l'adresse passé en paramètre
			// si aucun client n'est trouvé, alors il en créé un et l'ajoute à la liste
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

			// permet d'envoyer des données à une adresse spécifique
			void ClientUDP::sendTo(const sockaddr_storage& target, std::vector<uint8_t>&& data)
			{
				auto& client = getClient(target);
				//client.send(std::move(data));
			}

			// permet de récupérer les datagrammes sur le socket UDP
			// for(;;) permet de récupérer en permanence les socket s'il y en a
			// Ensuite ("onDatagramReceived()") permet de traiter et envoyer les socket au client
			void ClientUDP::receive()
			{
				for (;;)
				{
					Bousk::Network::UDP::Datagram datagram;
					sockaddr_in from{ 0 };
					socklen_t fromlen = sizeof(from);
					int ret = recvfrom(mSocket, reinterpret_cast<char*>(&datagram), Bousk::Network::UDP::Datagram::BufferMaxSize, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
					if (ret > 0)
					{
						if (ret > Bousk::Network::UDP::Datagram::HeaderSize)
						{
							datagram.datasize = ret - Bousk::Network::UDP::Datagram::HeaderSize;
							auto& client = getClient(reinterpret_cast<sockaddr_storage&>(from));
							client.onDatagramReceived(std::move(datagram));
						}
						else
						{
							//!< Datagramme inattendu
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
		}
	}
}