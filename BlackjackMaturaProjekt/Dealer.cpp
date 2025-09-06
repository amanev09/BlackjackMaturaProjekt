#include "Dealer.h"


// Prepare dealer for new round
void Dealer::clearHand() noexcept {
    hand.clear();
}

// Add card to dealer's hand
void Dealer::addCard(const Card& card) {
    hand.addCard(card);
}

Card Dealer::getWholeCard() const {
    return hand.getCard(1);
}

Card Dealer::getUpCard() const {
    return hand.getCard(0);
}

void Dealer::printStartingHand() { // prints the dealers hand in the format: UP_CARD; WHOLE_CARD
    //std::cout << hand.getCard(0).toString() << (r.peakAmericanStyle ? " WHOLECARD(" + std::to_string(hand.getCard(0).getValue()) + ")" : "") << std::endl;
}
// Access dealer's hand
Hand& Dealer::getHand() noexcept { return hand; }