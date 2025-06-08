#include "Player.h"

// Constructor to set player name and initialize one hand
Player::Player(const std::string& playerName) : name(playerName), balance(10000000){
    //hands.emplace_back();

}

void Player::setBalance(double balance) {
    this->balance = balance;
}

double Player::getBalance() const noexcept { return balance; }
// Add a card to the specified hand (default: first hand)
void Player::addCard(const Card& card, size_t handIndex) {
    if (handIndex < hands.size()) {
        hands[handIndex].addCard(card);
    }
}

Hand& Player::addHand() {
    hands.emplace_back(); // automatically creates a new hand object
    return hands.back();
}

void Player::removeHand(int i) {
    if (i >= 0 && static_cast<size_t>(i) < hands.size()) {
        hands.erase(hands.begin() + i);
    }
}


std::string Player::getName() const noexcept { return name; }

int Player::handCount() const noexcept {
    return static_cast<int>(hands.size());
}


// Access player's hands
std::vector<Hand>& Player::getHands() noexcept { return hands; }
const std::vector<Hand>& Player::getHands() const noexcept { return hands; }