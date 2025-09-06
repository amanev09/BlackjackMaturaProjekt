#pragma once
#include "Card.h"
#include "Hand.h"
#include "GameRules.h"

class Dealer {
    Hand hand; // second entry is hole card
    GameRules r;
public:
    // Prepare dealer for new round
    void clearHand() noexcept;

    // Add card to dealer's hand
    void addCard(const Card& card);

    Card getWholeCard() const;

    Card getUpCard() const;

    void printStartingHand();
    // Access dealer's hand
    Hand& getHand() noexcept;
};