#include <random>
#include <iostream>
#include <sstream> 
#include "Shoe.h"

Shoe::Shoe(int size) : deckSize(size), rng(std::random_device{}()) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
    reset();
}

// Shuffles deck using random number generator
void Shoe::shuffle() {

    std::shuffle(cards.begin(), cards.end(), rng); // Shuffle the deck
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

std::string Shoe::shoeKey() { // unique identification key for this shoe(mathematically same), 
    std::vector<int> counts(13,0); // this vector holds the values for all card counts from 1-A
    for (Card& c : cards) {
        // loop all cards, get their values and add to correct counts entry
        char rank = c.getRank();
        int val = c.getValue();
        if (val >= 2 && val <= 9) {
            counts[val - 2]++;
        }
        else if (val == 10) {
            if (rank == 'T') {
                counts[8]++;
            }
            else if (rank == 'J') {
                counts[9]++;
            }
            else if (rank == 'Q') {
                counts[10]++;
            }
            else if (rank == 'K') {
                counts[11]++;
            }
            else
                throw std::runtime_error("Invalid card value in the shoe");
        }
        else if (rank == 'A') {
            counts[12]++;
        }
        else {
            throw std::runtime_error("Invalid card value in shoe");
        }

    }
    // after we built the array containing values for the string, build the string$
    std::ostringstream key;
    for (int val : counts) {
        key << val << "_";
    }
    return key.str();
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
        std::cout << "Shuffeled Deck!";
    }
    Card dealtCard = cards.back();
    cards.pop_back();
    return dealtCard;
}

// Prints the current cards in the deck to the console
void Shoe::printShoe() const {
    for (const auto& card : cards) {
        std::cout << card.toString() << " ";
    }
    std::cout << std::endl;
}