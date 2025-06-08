#include <random>
#include <iostream>

#include "Shoe.h"

Shoe::Shoe(int size) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
    deckSize = size;
    reset();
}

// Shuffles deck using random number generator
void Shoe::shuffle() {
    std::random_device rd;              // Random seed
    std::mt19937 g(rd());               // Random number generator
    std::shuffle(cards.begin(), cards.end(), g); // Shuffle the deck
}

void Shoe::reset() {
    cards.clear();
    std::string ranks = "23456789TJQKA";
    std::string suits = "SHDC";
    for (int i = 0; i < deckSize; ++i) {
        for (char suit : suits) {
            for (char rank : ranks) {
                cards.emplace_back(rank, suit);
            }
        }
    }
    shuffle();
}

void Shoe::removeCard(char rank, char suit) {
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        if (it->getRank() == rank && it->getSuit() == suit) {
            cards.erase(it);
            return; // remove only first matching card
        }
    }
}


Card Shoe::deal() {
    if (cards.empty()) {
        reset();
        //std::cout << "Shuffeled Deck!";
    }
    Card dealtCard = cards.back();
    cards.pop_back();
    return dealtCard;
}

// Prints the current cards in the deck to the console
void Shoe::printDeck() const {
    for (const auto& card : cards) {
        //std::cout << card.toString() << " ";
    }
    //std::cout << std::endl;
}