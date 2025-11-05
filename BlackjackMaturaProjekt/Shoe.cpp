#include <random>
#include <iostream>
#include <sstream> 
#include "Shoe.h"
#include <cmath>
Shoe::Shoe(int size) : deckSize(size) , hiLoCount(0),dealtCardsCounter(0), 
cutCardPosition(0),tenValueCardsPlayed(0), rng(std::random_device{}()) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
    reset();
}

// Shuffles deck using random number generator
void Shoe::shuffle() {

    std::shuffle(cards.begin(), cards.end(), rng); // Shuffle the deck
    //std::cout << "Shuffeled the deck!\n";
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
    std::uniform_int_distribution<int> dist(static_cast<int>(cutCardLower * 52*deckSize),static_cast<int>(cutCardUpper * 52 * deckSize));
    cutCardPosition = dist(rng);
    dealtCardsCounter = 0;
    hiLoCount = 0;
    tenValueCardsPlayed = 0;
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

double Shoe::setHiLoTrueCount(double TC) {
    reset();
    if (TC == 0) return 0;
    int k = 0;

    while (k == 0) {
        reset();
        int RC = 0;
        int chosen = -1; // index from the back, how many cards until desired card comes(burn that too)
        k = 0;
        int totalCards = cards.size();
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        for (int i = cards.size() - 1; totalCards - i < cutCardPosition; --i) {
            if (i <= 0) break; 
            const Card& c = cards[i];
            if (c.getValue() <= 6) { RC++; } else if (c.getValue() >= 10) { RC--; }
            double t = 52.0* static_cast<double>(RC) / static_cast<double>(i); // i is the amount of cards left after we peek the first card, therefore use that for t
            if (t >= TC - 0.25 && t < TC + 0.25) {
                ++k;
                if (dist(rng) < 1.0 / k) {
                    chosen = totalCards - i;
                }
            }
        }
        if (k > 0) {
            for (int i = 0; i < chosen; ++i) {
                const Card& c = cards.back();
                cards.pop_back();
                dealtCardsCounter++;
                if (c.getValue() <= 6) { hiLoCount++; } else if (c.getValue() >= 10) { hiLoCount--; }
                if (c.getValue() == 10) { tenValueCardsPlayed++; }
            }
        }
    }
    shuffle(); // if we dont shuffle here, the first card after the burning to get the TC will be biased due to the nature of the algorithm. Shuffling ensures homogenous deck
    // shoe now has desired count using resevoir sampling
    return hiLoCount / (cards.size() / 52.0);
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
            if (it->getValue() == 10) { tenValueCardsPlayed++; }
            cards.erase(it);
            return; // remove only first matching card
        }
    }
}

double Shoe::getPTen() {
    double tCount = 0;
    for (Card& c : cards) {
        if (c.getValue() == 10) {
            tCount++;
        }
    }
    return tCount / static_cast<double>(cards.size());
    //return (static_cast<double>(deckSize * 16 - tenValueCardsPlayed)) / (static_cast<double>(cards.size()));
}

Card Shoe::deal() {
    dealtCardsCounter++;
    if (dealtCardsCounter == cutCardPosition) {
        //std::cout << "Cut card came, finish round and shuffle\n";
    }
    if (cards.empty()) {
        reset();
        //std::cout << "Emergency shuffle, shoe at 0 cards\n";
    }
    Card dealtCard = cards.back();
    cards.pop_back();
    // hiLo adjustment
    if (dealtCard.getValue() <= 6) { hiLoCount++; } else if (dealtCard.getValue() >= 10) { hiLoCount--; }
    if (dealtCard.getValue() == 10) { tenValueCardsPlayed++; }
    return dealtCard;
}

std::vector<Card> Shoe::getCardsVector() {
    return cards;
}

// Prints the current cards in the deck to the console
void Shoe::printShoe() const {
    for (const auto& card : cards) {
        //std::cout << card.toString() << " ";
    }
    //std::cout << std::endl;
}