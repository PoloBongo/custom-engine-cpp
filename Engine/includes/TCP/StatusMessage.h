#pragma once
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

    // Méthodes pour modifier l'état
    void setStatus(Status newStatus) {
        status_ = newStatus;
    }

    Status getStatus() const {
        return status_;
    }

private:
    StatusMessage() : status_(None) {} // Initialisation par défaut à None
    Status status_;
};