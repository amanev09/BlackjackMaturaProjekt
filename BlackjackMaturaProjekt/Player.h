#pragma once
#include <string>
#include "Hand.h"

class Player {
    std::string name;
    std::vector<Hand> hands;
    double balance;
public:
    // Constructor to set player name and initialize one hand
    Player(const std::string& playerName);

    void setBalance(double balance);

    double getBalance() const noexcept;
    // Add a card to the specified hand (default: first hand)
    void addCard(const Card& card, size_t handIndex = 0);

    Hand& addHand();

    void removeHand(int i);


    std::string getName() const noexcept;

    int handCount() const noexcept;


    // Access player's hands
    std::vector<Hand>& getHands() noexcept;
    const std::vector<Hand>& getHands() const noexcept;
};