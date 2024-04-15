#include "UDP/Packet.h"
#include "UDP/Utils.h"
#include <cassert>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			void Multiplexer::queue(std::vector<uint8_t>&& msgData)
			{
				//<! S’assurer que notre message ne dépasse pas la limite auto-imposée
				assert(msgData.size() <= Packet::MaxMessageSize);
				if (msgData.size() > Packet::DataMaxSize)
				{
					size_t queuedSize = 0;
					while (queuedSize < msgData.size())
					{
						const auto fragmentSize = std::min(Packet::DataMaxSize, static_cast<uint16_t>(msgData.size() - queuedSize));
						Packet packet;
						packet.header.id = mNextId++;
						packet.header.type = ((queuedSize == 0) ? Packet::Type::FirstFragment : Packet::Type::Fragment);
						packet.header.size = fragmentSize;
						memcpy(packet.data(), msgData.data() + queuedSize, fragmentSize);
						mQueue.push_back(packet);
						queuedSize += fragmentSize;
					}
					mQueue.back().header.type = Packet::Type::LastFragment;
					assert(queuedSize == msgData.size());
				}
				else
				{
					//!< Message n’ayant pas besoin d’être découpé
					Packet packet;
					packet.header.id = mNextId++;
					packet.header.type = Packet::Type::Packet;
					packet.header.size = static_cast<uint16_t>(msgData.size());
					memcpy(packet.data(), msgData.data(), msgData.size());
					mQueue.push_back(packet);
				}
			}

			size_t Multiplexer::serialize(uint8_t* buffer, const size_t buffersize)
			{
				size_t serializedSize = 0;
				for (auto packetit = mQueue.cbegin(); packetit != mQueue.cend();)
				{
					const auto& packet = *packetit;
					if (serializedSize + packet.size() > buffersize)
						break; //<! Pas assez d’espace libre dans le tampon pour inclure ce paquet

					memcpy(buffer, packet.buffer(), packet.size());
					serializedSize += packet.size();
					buffer += packet.size();

					//<! Une fois qu’un paquet a été sérialisé, on le retire de la file d’envoi
					packetit = mQueue.erase(packetit);
				}
				return serializedSize;
			}

			void Demultiplexer::onDataReceived(const uint8_t* data, const size_t datasize)
			{
				//<! Extraire les paquets du tampon
				size_t processedDataSize = 0;
				while (processedDataSize < datasize)
				{
					const Packet* pckt = reinterpret_cast<const Packet*>(data);
					if (processedDataSize + pckt->size() > datasize || pckt->datasize() > Packet::DataMaxSize)
					{
						//<! Paquet ou tampon mal formé, on interrompt
						return;
					}
					onPacketReceived(pckt);
					processedDataSize += pckt->size();
					data += pckt->size();
				}
			}

			void Demultiplexer::onPacketReceived(const Packet* pckt)
			{
				if (!Utils::IsSequenceNewer(pckt->id(), mLastProcessed))
					return; //<! Paquet trop vieux

				//<! Trouver l’emplacement dans la liste pour ce paquet, notre liste doit rester ordonnée
				if (mPendingQueue.empty() || Utils::IsSequenceNewer(pckt->id(), mPendingQueue.back().id()))
				{
					mPendingQueue.push_back(*pckt);
				}
				else
				{
					//<! Trouver le premier itérateur avec un identifiant égal à ou plus récent que notre paquet, nous devons placer le paquet avant celui-ci
					auto insertLocation = std::find_if(mPendingQueue.cbegin(), mPendingQueue.cend(), [&pckt](const Packet& p) { return p.id() == pckt->id() || Utils::IsSequenceNewer(p.id(), pckt->id()); });
					//<! Assurons-nous de ne pas insérer un paquet en doublon
					if (insertLocation->id() != pckt->id())
					{
						mPendingQueue.insert(insertLocation, *pckt);
					}
				}
			}

			std::vector<std::vector<uint8_t>> Demultiplexer::process()
			{
				std::vector<std::vector<uint8_t>> messagesReady;

				auto itPacket = mPendingQueue.cbegin();
				auto itEnd = mPendingQueue.cend();
				std::vector<Packet>::const_iterator newestProcessedPacket;
				while (itPacket != itEnd)
				{
					if (itPacket->type() == Packet::Type::Packet)
					{
						//<! Paquet-message, on recopie toutes ses données
						std::vector<uint8_t> msg(itPacket->data(), itPacket->data() + itPacket->datasize());
						messagesReady.push_back(std::move(msg));
						newestProcessedPacket = itPacket;
						//<! On N’oublie pas d’incrémenter l’itérateur pour passer au paquet suivant
						++itPacket;
					}
					else if (itPacket->type() == Packet::Type::FirstFragment)
					{
						//<! Il faut vérifier que le message est prêt, s’il est entièrement reçu
						std::vector<uint8_t> msg = [&]()
							{
								std::vector<uint8_t> msg(itPacket->data(), itPacket->data() + itPacket->datasize());
								//<! Ceci est l’identifiant de paquet à suivre, il s’incrémentera avec le parcours de la liste
								auto expectedPacketId = itPacket->id();
								//<! Le premier paquet est le premier fragment, on passe directement au suivant
								++itPacket;
								++expectedPacketId;
								//<! Tant que nous avons des paquets dans la liste et qu’ils ont l’identifiant voulu, on traite le paquet
								while (itPacket != itEnd && itPacket->id() == expectedPacketId)
								{
									if (itPacket->type() == Packet::Type::LastFragment)
									{
										//<! Dernier fragment atteint, le message est complet
										msg.insert(msg.cend(), itPacket->data(), itPacket->data() + itPacket->datasize());
										return msg;
									}
									else if (itPacket->type() != Packet::Type::Fragment)
									{
										//<! Si on rentre ici, nous avons reçu un paquet mal formé ou mal intentionné
										msg.clear();
										return msg;
									}
									//<! Il s’agit d’un fragment valide à extraire et insérer dans le message final
									msg.insert(msg.cend(), itPacket->data(), itPacket->data() + itPacket->datasize());
									++itPacket;
									++expectedPacketId;
								}
								//<! Si on arrive ici, le message est incomplet ou a une partie manquante
								msg.clear();
								return msg;
							}();
							if (!msg.empty())
							{
								//<! Nous avons un message !
								messagesReady.push_back(std::move(msg));
								newestProcessedPacket = itPacket;
								//<! On déplace l’itérateur de parcours de la liste après le dernier paquet traité, qui est le dernier paquet du message
								++itPacket;
							}
					}
					else
					{
						//<! Ce paquet est un fragment que l’on ne peut pas traiter, on le saute
						++itPacket;
					}
				}


				//<! Si des messages ont été extraits, on doit mettre à jour notre état interne
				if (!messagesReady.empty())
				{
					//<! Mise à jour du dernier paquet traité, afin de refuser les plus anciens par la suite (protocole ordonné non-fiable)
					mLastProcessed = newestProcessedPacket->id();
					//<! Retirer tous les paquets traités de la file, y compris le dernier traité
					mPendingQueue.erase(mPendingQueue.cbegin(), std::next(newestProcessedPacket));
				}

				return messagesReady;
			}
		}
	}
}