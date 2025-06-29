#include "Hand.h"


void Hand::addCard(const Card& card) {
    cards.push_back(card); // adds card to hand
}

Card Hand::removeCard(int i) {
    if (i < 0 || i >= cards.size()) {
        throw std::out_of_range("Index out of range");
    }
    Card removed = cards[i];
    cards.erase(cards.begin() + i);
    return removed;
}

void Hand::clear() noexcept {
    cards.clear();
}

void Hand::reset() {
    clear();
    bet = 0;
    flagIsInTheGame = true;
    flagIsHandResolved = false;
    flagEvenMoney = false;
    flagSurrendered = false;
    flagIsSplitHand = false;
    sideBets.clear(); // fully remove any sidebets. no need to zero them because entries get created automatically when its empty
}

bool Hand::getFlagIsInTheGame() const noexcept { return flagIsInTheGame; }
void Hand::setFlagIsInTheGame(bool flag) { flagIsInTheGame = flag; }
bool Hand::getFlagIsHandResolved() const noexcept { return flagIsHandResolved; }
void Hand::setFlagIsHandResolved(bool flag) { flagIsHandResolved = flag; }
void Hand::setFlagEvenMoney(bool flag) { flagEvenMoney = flag; }
bool Hand::getFlagEvenMoney() const noexcept { return flagEvenMoney; }
void Hand::setFlagSurrendered(bool flag) { flagSurrendered = flag; }
bool Hand::getFlagSurrendered() const noexcept { return flagSurrendered; }
void Hand::setFlagIsSplitHand(bool flag) { flagIsSplitHand = flag; }
bool Hand::getFlagIsSplitHand() const noexcept { return flagIsSplitHand; }

void Hand::setSideBet(const std::string& betName, double amount) {
    sideBets[betName] = amount;
}

double Hand::getSideBet(const std::string& betName) const {
    auto it = sideBets.find(betName);
    return it != sideBets.end() ? it->second : 0; // returns 0 if not set
}

double Hand::getBet() const noexcept {
    return bet;
}

void Hand::setBet(double bet) noexcept {
    this->bet = bet;
}

int Hand::getValue() const noexcept { // returns value of all Hands adjusted for Aces
    int totalValue = 0;
    int softAceCount = 0;

    for (const Card& card : cards) {
        int cardValue = card.getValue();
        totalValue += cardValue;
        if (cardValue == 11) {
            softAceCount++;
        }
    }
    while (totalValue > 21 && softAceCount > 0) {
        // treat soft aces hard until below 21 or no more soft aces in hand
        totalValue -= 10;
        softAceCount--;
    }
    return totalValue;
}

bool Hand::isSoft() { // determines if hand is soft(contains aces counted as 11)
    int totalValue = 0;
    int softAceCount = 0;

    for (const Card& card : cards) {
        int cardValue = card.getValue();
        totalValue += cardValue;
        if (cardValue == 11) {
            softAceCount++;
        }
    }
    while (totalValue > 21 && softAceCount > 0) {
        // treat soft aces hard until below 21 or no more soft aces in hand
        totalValue -= 10;
        softAceCount--;
    }
    if (softAceCount > 0) {
        return true;
    }
    return false;
}



Card Hand::getCard(int i) const { // returns card for given index i
    return cards[i];
}

int Hand::getCardsCount() {
    return static_cast<int>(cards.size());
}

bool Hand::isSplittable() {
    return cards.size() == 2 and (getCard(0).getRank() == getCard(1).getRank());
}

void Hand::printHand() const noexcept {
    for (const Card& card : cards) {
        //std::cout << card.getRank() << card.getSuit() << " ";
    }
    //std::cout << "(" << this->getValue() << ")" << std::endl;
}