#pragma once
#include <vector>
#include "Card.h"

class Shoe {
    std::vector<Card> cards; // Holds all cards in shoe
    int deckSize;
    std::mt19937 rng;
public:
    Shoe(int size = 1);

    // Shuffles deck using random number generator
    void shuffle();

    void reset();


    Card deal();

    void removeCard(char rank, char suit);

    // Prints the current cards in the deck to the console
    void printShoe() const;
};