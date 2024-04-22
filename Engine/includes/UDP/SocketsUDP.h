#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <winSock2.h>
#include <ws2tcpip.h>
using nfds_t = unsigned long;
inline int poll(pollfd fdarray[], nfds_t nfds, int timeout) { return WSAPoll(fdarray, nfds, timeout); }
#endif

#include <string>

namespace Bousk
{
    namespace Network
    {
        /**
         * @brief Initialise les sockets.
         * @return true si l'initialisation r�ussit, sinon false.
         */
        bool Start();

        /**
         * @brief Lib�re les ressources associ�es aux sockets.
         */
        void Release();

        /**
         * @brief Configure un socket en mode non bloquant.
         * @param socket Socket � configurer.
         * @return true si la configuration r�ussit, sinon false.
         */
        bool SetNonBlocking(SOCKET socket);

        /**
         * @brief Configure un socket pour r�utiliser l'adresse.
         * @param socket Socket � configurer.
         * @return true si la configuration r�ussit, sinon false.
         */
        bool SetReuseAddr(SOCKET socket);

        /**
         * @brief Ferme un socket.
         * @param socket Socket � fermer.
         */
        void CloseSocket(SOCKET socket);

        /**
         * @brief R�cup�re l'adresse IP d'une structure sockaddr_in.
         * @param addr Structure sockaddr_in contenant l'adresse.
         * @return L'adresse IP sous forme de cha�ne de caract�res.
         */
        std::string GetAddress(const sockaddr_in& addr);

        /**
         * @brief R�cup�re le port d'une structure sockaddr_in.
         * @param addr Structure sockaddr_in contenant le port.
         * @return Le port sous forme d'entier non sign�.
         */
        unsigned short GetPort(const sockaddr_in& addr);
    }
}