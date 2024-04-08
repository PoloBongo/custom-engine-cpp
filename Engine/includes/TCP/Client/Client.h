#pragma once

#include "TCP/Sockets.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <cassert>
#include <numeric>

namespace Network
{
    namespace Messages {
        class Base;
    }
    namespace TCP
    {
        /**
         * @brief Type de donn�es utilis� pour l'en-t�te des messages.
         */
        using HeaderType = uint16_t;

        /**
         * @brief Taille de l'en-t�te des messages en octets.
         */
        static const unsigned int HeaderSize = sizeof(HeaderType);

        /**
         * @brief Impl�mentation priv�e de la classe Client.
         */
        class ClientImpl;

        /**
         * @brief La classe Client repr�sente un client TCP.
         */
        class Client
        {
        public:
            /**
             * @brief Constructeur par d�faut de la classe Client.
             */
            Client() {}

            /**
             * @brief Constructeur de copie supprim�.
             */
            Client(const Client&) = delete;

            /**
             * @brief Op�rateur d'assignation de copie supprim�.
             */
            Client& operator=(const Client&) = delete;

            /**
             * @brief Constructeur de d�placement de la classe Client.
             */
            Client(Client&&);

            /**
             * @brief Op�rateur d'assignation par d�placement de la classe Client.
             */
            Client& operator=(Client&&);

            /**
             * @brief Destructeur de la classe Client.
             */
            ~Client() {}

            /**
             * @brief Initialise le client avec le socket et l'adresse sp�cifi�s.
             * @param sckt Socket � associer au client.
             * @param addr Adresse � associer au client.
             * @return true si l'initialisation r�ussit, sinon false.
             */
            bool init(SOCKET&& sckt, const sockaddr_in& addr);

            /**
             * @brief �tablit une connexion avec le serveur � l'adresse et au port sp�cifi�s.
             * @param ipaddress Adresse IP du serveur.
             * @param port Port du serveur.
             * @return true si la connexion r�ussit, sinon false.
             */
            bool connect(const std::string& ipaddress, unsigned short port);

            /**
             * @brief Ferme la connexion avec le serveur.
             */
            void disconnect();

            /**
             * @brief Envoie des donn�es au serveur.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool send(const unsigned char* data, unsigned int len);

            /**
             * @brief R�cup�re les messages entrants du serveur.
             * @return Un pointeur unique vers le message re�u.
             */
            std::unique_ptr<Messages::Base> poll();

            /**
             * @brief R�cup�re l'identifiant du client.
             * @return L'identifiant du client.
             */
            uint64_t id() const;

            /**
             * @brief R�cup�re l'adresse de destination du client.
             * @return L'adresse de destination du client.
             */
            const sockaddr_in& destinationAddress() const;

        private:
            std::unique_ptr<ClientImpl> mImpl; /**< Impl�mentation priv�e du client. */
        };


        class ConnectionHandler
        {
        public:
            ConnectionHandler() = default;

            /**
             * @brief �tablit une connexion avec l'adresse et le port sp�cifi�s.
             * @param sckt Socket � utiliser pour la connexion.
             * @param address Adresse IP du serveur.
             * @param port Port du serveur.
             * @return true si la connexion r�ussit, sinon false.
             */
            bool connect(SOCKET sckt, const std::string& address, unsigned short port);

            /**
             * @brief R�cup�re les messages de connexion.
             * @return Un pointeur unique vers le message de connexion re�u.
             */
            std::unique_ptr<Messages::Connection> poll();

            /**
             * @brief Renvoie l'adresse � laquelle la connexion est �tablie.
             * @return L'adresse � laquelle la connexion est �tablie.
             */
            const sockaddr_in& connectedAddress() const { return mConnectedAddress; }

        private:
            pollfd mFd = { 0 }; /**< Permet d'associer le descripteur de fichier � la connexion. */
            sockaddr_in mConnectedAddress; /**< Adresse IP + port auquel la connexion est �tablie. */
            std::string mAddress; /**< Adresse IP + port auquel doit �tre �tablie la connexion. */
            unsigned short mPort; /**< Port auquel doit �tre �tablie la connexion. */
        };

        class ReceptionHandler
        {
            enum class State {
                Header, /**< L'en-t�te du message qui contient des m�tadonn�es sur le message. */
                Data, /**< Les donn�es comme du texte, des images, etc. (ce que l'utilisateur enverra). */
            };
        public:
            ReceptionHandler() = default;

            /**
             * @brief Initialise le gestionnaire de r�ception avec le socket sp�cifi�.
             * @param sckt Socket � utiliser pour la r�ception.
             */
            void init(SOCKET sckt);

            /**
             * @brief Re�oit les messages entrants.
             * @return Un pointeur unique vers le message re�u.
             */
            std::unique_ptr<Messages::Base> recv();

        private:
            /**
             * @brief Pointeur vers le d�but des donn�es manquantes dans le tampon de r�ception.
             * @return Pointeur vers le d�but des donn�es manquantes.
             */
            inline char* missingDataStartBuffer() { return reinterpret_cast<char*>(mBuffer.data() + mReceived); }

            /**
             * @brief Calcule la longueur des donn�es manquantes dans le tampon de r�ception.
             * @return Longueur des donn�es manquantes.
             */
            inline int missingDataLength() const { return static_cast<int>(mBuffer.size() - mReceived); }

            /**
             * @brief D�marre la r�ception de l'en-t�te du message.
             */
            void startHeaderReception();

            /**
             * @brief D�marre la r�ception des donn�es du message.
             */
            void startDataReception();

            /**
             * @brief D�marre la r�ception en sp�cifiant la longueur attendue des donn�es.
             * @param expectedDataLength Longueur attendue des donn�es.
             * @param newState Nouvel �tat de la r�ception.
             */
            void startReception(unsigned int expectedDataLength, State newState);

        private:
            std::vector<unsigned char> mBuffer; /**< Tampon de r�ception des donn�es. */
            unsigned int mReceived; /**< Nombre de donn�es d�j� re�ues. */
            SOCKET mSckt{ INVALID_SOCKET }; /**< Socket utilis� pour la r�ception. */
            State mState; /**< �tat actuel de la r�ception. */
        };


        class SendingHandler
        {
            enum class State {
                Idle, /**< �tat inactif. */
                Header, /**< �tat de l'en-t�te. */
                Data, /**< �tat des donn�es. */
            };
        public:
            SendingHandler() = default;

            /**
             * @brief Initialise le gestionnaire d'envoi avec le socket sp�cifi�.
             * @param sckt Socket � utiliser pour l'envoi.
             */
            void init(SOCKET sckt);

            /**
             * @brief Envoie des donn�es au serveur.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param datalen Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool send(const unsigned char* data, unsigned int datalen);

            /**
             * @brief Met � jour le gestionnaire d'envoi.
             */
            void update();

            /**
             * @brief Renvoie la taille de la file d'attente d'envoi.
             * @return Taille de la file d'attente.
             */
            size_t queueSize() const;

        private:
            /**
             * @brief Envoie le tampon en attente.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool sendPendingBuffer();

            /**
             * @brief Pr�pare l'en-t�te du prochain message � envoyer.
             */
            void prepareNextHeader();

            /**
             * @brief Pr�pare les donn�es du prochain message � envoyer.
             */
            void prepareNextData();

        private:
            std::list<std::vector<unsigned char>> mQueueingBuffers; /**< File d'attente des tampons � envoyer. */
            std::vector<unsigned char> mSendingBuffer; /**< Tampon actuellement en cours d'envoi. */
            SOCKET mSocket{ INVALID_SOCKET }; /**< Socket associ� au gestionnaire d'envoi. */
            State mState{ State::Idle }; /**< �tat actuel du gestionnaire d'envoi. */
        };


        class ClientImpl
        {
            enum class State {
                Connecting,
                Connected,
                Disconnected,
            };

        public:
            ClientImpl() = default;
            ~ClientImpl();

            /**
             * @brief Initialise la connexion client avec le socket et l'adresse sp�cifi�s.
             * @param sckt Socket � associer au client.
             * @param addr Adresse � associer au client.
             * @return true si l'initialisation r�ussit, sinon false.
             */
            bool init(SOCKET&& sckt, const sockaddr_in& addr);

            /**
             * @brief �tablit une connexion avec le serveur � l'adresse et au port sp�cifi�s.
             * @param ipaddress Adresse IP du serveur.
             * @param port Port du serveur.
             * @return true si la connexion r�ussit, sinon false.
             */
            bool connect(const std::string& ipaddress, unsigned short port);

            /**
             * @brief Ferme la connexion avec le serveur.
             */
            void disconnect();

            /**
             * @brief Envoie des donn�es au serveur.
             * @param data Pointeur vers les donn�es � envoyer.
             * @param len Longueur des donn�es � envoyer.
             * @return true si l'envoi r�ussit, sinon false.
             */
            bool send(const unsigned char* data, unsigned int len);

            /**
             * @brief R�cup�re les messages entrants du serveur.
             * @return Un pointeur unique vers le message re�u.
             */
            std::unique_ptr<Messages::Base> poll();

            /**
             * @brief R�cup�re l'identifiant du client.
             * @return L'identifiant du client.
             */
            uint64_t id() const { return static_cast<uint64_t>(mSocket); }

            /**
             * @brief R�cup�re l'adresse de destination du client.
             * @return L'adresse de destination du client.
             */
            const sockaddr_in& destinationAddress() const { return mAddress; }

        private:
            /**
             * @brief Callback appel� lorsqu'une connexion est �tablie avec succ�s.
             * @param addr Adresse du serveur connect�.
             */
            void onConnected(const sockaddr_in& addr);

        private:
            ConnectionHandler mConnectionHandler;
            SendingHandler mSendingHandler;
            ReceptionHandler mReceivingHandler;
            sockaddr_in mAddress{ 0 }; /**< Adresse associ�e au client. */
            SOCKET mSocket{ INVALID_SOCKET }; /**< Socket associ� au client. */
            State mState{ State::Disconnected }; /**< �tat actuel du client. */
        };
    }
}