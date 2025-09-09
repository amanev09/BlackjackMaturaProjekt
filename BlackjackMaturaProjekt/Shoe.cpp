#include <random>
#include <iostream>
#include <sstream> 
#include "Shoe.h"
#include <cmath>
Shoe::Shoe(int size) : deckSize(size) , hiLoCount(0),dealtCardsCounter(0), 
cutCardPosition(0), rng(std::random_device{}()) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
    reset();
}

// Shuffles deck using random number generator
void Shoe::shuffle() {

    std::shuffle(cards.begin(), cards.end(), rng); // Shuffle the deck
    std::cout << "Shuffeled the deck!\n";
}

void Shoe::onNewGameRound() {
    // quick check if cut card came already, function called whenever new game starts.
    if (dealtCardsCounter >= cutCardPosition) {
        reset();
    }
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
    std::uniform_int_distribution<int> dist(static_cast<int>(0.75 * 52*deckSize),static_cast<int>(0.85 * 52 * deckSize));
    cutCardPosition = dist(rng);
    dealtCardsCounter = 0;
    hiLoCount = 0;
}

int Shoe::getHiLoCount() {
    return hiLoCount;
}

int Shoe::getHiLoTrueCount() {
    if (cards.empty()) return 0;
    double decksRemaining = cards.size() / 52.0;
    double tc = hiLoCount / decksRemaining;
    return static_cast<int>(std::round(tc));
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
            // adjust HiLo
            if (it->getValue() <= 6) { hiLoCount++; } else if (it->getValue() >= 10) { hiLoCount--; }
            cards.erase(it);
            return; // remove only first matching card
        }
    }
}


Card Shoe::deal() {
    dealtCardsCounter++;
    if (dealtCardsCounter == cutCardPosition) {
        std::cout << "Cut card came, finish round and shuffle\n";
    }
    if (cards.empty()) {
        reset();
        std::cout << "Emergency shuffle, shoe at 0 cards\n";
    }
    Card dealtCard = cards.back();
    cards.pop_back();
    // hiLo adjustment
    if (dealtCard.getValue() <= 6) { hiLoCount++; } else if (dealtCard.getValue() >= 10) { hiLoCount--; }
    return dealtCard;
}

// Prints the current cards in the deck to the console
void Shoe::printShoe() const {
    for (const auto& card : cards) {
        std::cout << card.toString() << " ";
    }
    std::cout << std::endl;
}