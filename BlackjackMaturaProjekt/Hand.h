#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>

#include "Card.h"


class Hand {
    std::vector<Card> cards; // cards in hand
    double bet = 0; // bet is specific to each hand, not player
    bool flagIsInTheGame = true; // is the hand still in the game and therefore needs to be prompted for actions?
    bool flagIsHandResolved = false; // has the hand already been resolved? If not, we need to resolve it at the end of the game. Important for surrender
    bool flagEvenMoney = false; // did the player take even money for this hand
    bool flagSurrendered = false; // did the player surrender the hand
    bool flagIsSplitHand = false; // is this hand the result of splitting a previous hand
    std::unordered_map<std::string, double> sideBets; // dictionary for the sidebet and respective bet amount. includes but not limited to insurance, even money
public:
    void addCard(const Card& card);

    Card removeCard(int i);

    void clear() noexcept;

    void reset();

    bool getFlagIsInTheGame() const noexcept;
    void setFlagIsInTheGame(bool flag);
    bool getFlagIsHandResolved() const noexcept;
    void setFlagIsHandResolved(bool flag);
    void setFlagEvenMoney(bool flag);
    bool getFlagEvenMoney() const noexcept;
    void setFlagSurrendered(bool flag);
    bool getFlagSurrendered() const noexcept;
    void setFlagIsSplitHand(bool flag);
    bool getFlagIsSplitHand() const noexcept;

    void setSideBet(const std::string& betName, double amount);

    double getSideBet(const std::string& betName) const;
    double getBet() const noexcept;

    void setBet(double bet) noexcept;

    int getValue() const noexcept;

    bool isSoft();

    Card getCard(int i = 0) const;

    int getCardsCount();

    void printHand() const noexcept;
};