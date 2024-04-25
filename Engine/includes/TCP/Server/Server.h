#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"

#include <memory>
#include <map>
#include <list>
#include <cassert>

namespace Network
{
    namespace Messages {
        class Base;
    }
    namespace TCP
    {
        /**
         * @brief Impl�mentation priv�e de la classe Server.
         */
        class ServerImpl;

        /**
         * @brief La classe Server repr�sente un serveur TCP.
         */
        class Server
        {
        public:
            /**
             * @brief Constructeur par d�faut de la classe Server.
             */
            Server() {}

            /**
             * @brief Constructeur de copie supprim�.
             */
            Server(const Server&) = delete;

            /**
             * @brief Op�rateur d'assignation de copie supprim�.
             */
            Server& operator=(const Server&) = delete;

            /**
             * @brief Constructeur de d�placement de la classe Server.
             */
            Server(Server&&);

            /**
             * @brief Op�rateur d'assignation par d�placement de la classe Server.
             */
            Server& operator=(Server&&);

            /**
             * @brief Destructeur de la classe Server.
             */
            ~Server() {}

            /**
             * @brief D�marre le serveur sur le port sp�cifi�.
             * @param _port Port sur lequel d�marrer le serveur.
             * @return true si le d�marrage r�ussit, sinon false.
             */
            bool start(unsigned short _port);

            /**
             * @brief Arr�te le serveur.
             */
            void stop();

            /**
             * @brief Met � jour le serveur pour traiter les �v�nements en attente.
             */
            void update();

            /**
             * @brief Envoie des donn�es � un client sp�cifi�.
             * @param clientid Identifiant du client.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool sendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len);

            /**
             * @brief Envoie des donn�es � tous les clients connect�s.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool sendToAll(const unsigned char* _data, unsigned int _len);

            /**
             * @brief R�cup�re les messages entrants du serveur.
             * @return Un pointeur unique vers le message re�u.
             */
            std::unique_ptr<Messages::Base> poll();

        private:
            std::unique_ptr<ServerImpl> mImpl; /**< Impl�mentation priv�e du serveur. */
        };

        class ServerImpl
        {
        public:
            ServerImpl() = default;
            ~ServerImpl();

            /**
             * @brief D�marre le serveur sur le port sp�cifi�.
             * @param _port Port sur lequel d�marrer le serveur.
             * @return true si le d�marrage r�ussit, sinon false.
             */
            bool start(unsigned short _port);

            /**
             * @brief Arr�te le serveur.
             */
            void stop();

            /**
             * @brief Met � jour le serveur.
             */
            void update();

            /**
             * @brief Envoie des donn�es � un client sp�cifique.
             * @param clientid Identifiant du client.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool sendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len);

            /**
             * @brief Envoie des donn�es � tous les clients connect�s.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool sendToAll(const unsigned char* _data, unsigned int _len);

            /**
             * @brief R�cup�re les messages entrants.
             * @return Un pointeur unique vers le message re�u.
             */
            std::unique_ptr<Messages::Base> poll();

        private:
            std::map<uint64_t, Client> mClients; /**< Map des clients connect�s, index�s par leur identifiant. */
            std::list<std::unique_ptr<Messages::Base>> mMessages; /**< Liste des messages entrants. */
            SOCKET mSocket{ INVALID_SOCKET }; /**< Socket du serveur. */
        };
    }
}