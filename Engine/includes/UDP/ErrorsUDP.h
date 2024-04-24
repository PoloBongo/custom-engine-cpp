#pragma once

#ifdef _WIN32
#include <winSock2.h>
#else
#include <cerrno>
#define SOCKET int
#define INVALID_SOCKET ((int)-1)
#define SOCKET_ERROR (int(-1))
#endif


namespace Bousk
{
    namespace Network
    {
        namespace ErrorsUDP
        {
            /**
             * @brief R�cup�re le code d'erreur du dernier appel r�seau.
             * @return Le code d'erreur du dernier appel r�seau.
             */
            int Get();

            /**
             * @brief Enum�ration des codes d'erreur r�seau courants.
             */
            enum {
                AGAIN = WSATRY_AGAIN, /**< Ressayer l'op�ration. */
                WOULDBLOCK = WSAEWOULDBLOCK, /**< L'op�ration bloquerait le socket. */
                INPROGRESS = WSAEINPROGRESS, /**< L'op�ration est en cours. */
                INTR = WSAEINTR, /**< L'appel syst�me a �t� interrompu avant qu'il ne se termine. */
            };
        }
    }
}