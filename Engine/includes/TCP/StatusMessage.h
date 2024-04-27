#pragma once

#include <vector>
#include <string>

class StatusMessage
{
public:
    enum Status
    {
        Send,
        None
    };

    // Méthode pour obtenir l'instance unique de StatusMessage
    static StatusMessage& getInstance() {
        static StatusMessage instance; // Crée une seule instance de StatusMessage
        return instance;
    }

    void setStatus(Status newStatus) {
        status_ = newStatus;
    }

    Status getStatus() const {
        return status_;
    }

    const std::vector<std::pair<std::string, std::string>>& getReceivedMessages() const {
        return receivedMessages;
    }

    void addMessage(const std::string& message, const std::string& pseudo) {
        receivedMessages.emplace_back(message, pseudo);
    }

    std::string getHostName();

    void setPseudo(std::string _pseudo) {
        pseudo = _pseudo;
    }

    std::string getPseudo() const {
        return pseudo;
    }

private:
    StatusMessage() : status_(None) {} // Initialisation par défaut à None
    ~StatusMessage() {} // Destructeur privé pour empêcher la destruction externe

    StatusMessage(const StatusMessage&) = delete;              // Supprime la copie
    StatusMessage& operator=(const StatusMessage&) = delete;

    std::vector<std::pair<std::string, std::string>> receivedMessages;
    Status status_;
    std::string pseudo;
};