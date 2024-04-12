#pragma once

#include <vector>
#include <winSock2.h>

namespace Network
{
    namespace Messages
    {
        /**
         * @brief Classe de base pour les messages r�seau.
         */
        class Base
        {
        public:
            /**
             * @brief V�rifie si le message est de type M.
             * @tparam M Type de message � v�rifier.
             * @return true si le message est de type M, sinon false.
             */
            template<class M>
            bool is() const { return mType == M::StaticType; }

            /**
             * @brief Convertit le message en type M.
             * @tparam M Type de message � convertir.
             * @return Pointeur vers le message converti en type M.
             */
            template<class M>
            const M* as() const { return static_cast<const M*>(this); }

            sockaddr_in from; /**< Adresse du destinataire du message. */
            uint64_t idFrom; /**< Identifiant du destinataire du message. */

        protected:
            /**
             * @brief Type de message.
             */
            enum class Type {
                Connection, /**< Message de connexion. */
                Disconnection, /**< Message de d�connexion. */
                UserData, /**< Message de donn�es utilisateur. */
            };

            /**
             * @brief Constructeur de la classe Base.
             * @param type Type du message.
             */
            Base(Type type)
                : mType(type)
            {}

        private:
            Type mType; /**< Type du message. */
        };

        /**
         * @brief Message de connexion.
         */
        class Connection : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::Connection;

        public:
            /**
             * @brief R�sultat de la connexion.
             */
            enum class Result {
                Success, /**< Connexion r�ussie. */
                Failed, /**< Connexion �chou�e. */
            };

            /**
             * @brief Constructeur de la classe Connection.
             * @param r R�sultat de la connexion.
             */
            Connection(Result r)
                : Base(Type::Connection)
                , result(r)
            {}

            Result result; /**< R�sultat de la connexion. */
        };

        /**
         * @brief Message de d�connexion.
         */
        class Disconnection : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::Disconnection;

        public:
            /**
             * @brief Raison de la d�connexion.
             */
            enum class Reason {
                Disconnected, /**< D�connexion. */
                Lost, /**< Connexion perdue. */
            };

            /**
             * @brief Constructeur de la classe Disconnection.
             * @param r Raison de la d�connexion.
             */
            Disconnection(Reason r)
                : Base(Type::Disconnection)
                , reason(r)
            {}

            Reason reason; /**< Raison de la d�connexion. */
        };

        /**
         * @brief Message de donn�es utilisateur.
         */
        class UserData : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::UserData;

        public:
            /**
             * @brief Constructeur de la classe UserData.
             * @param d Donn�es utilisateur.
             */
            UserData(std::vector<unsigned char>&& d)
                : Base(Type::UserData)
                , data(std::move(d))
            {}

            std::vector<unsigned char> data; /**< Donn�es utilisateur. */
        };
    }
}