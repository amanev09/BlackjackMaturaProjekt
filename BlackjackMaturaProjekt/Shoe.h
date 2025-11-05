#pragma once
#include <vector>
#include "Card.h"
#include <random>
class Shoe {
    std::vector<Card> cards; // Holds all cards in shoe
    int deckSize;
    std::mt19937 rng;
    int hiLoCount;
    int dealtCardsCounter;
    int tenValueCardsPlayed;
    double cutCardPosition;
    double cutCardLower = 0.75;
    double cutCardUpper = 0.85;
public:
    Shoe(int size = 1);

    // Shuffles deck using random number generator
    void shuffle();

    void onNewGameRound();

    void reset();

    double setHiLoTrueCount(double trueCount); // double because we want to see EV difference between TC of 3 and 3.5 too, bad fractional TC's might be difficult to achieve naturally

    std::string shoeKey();

    Card deal();

    void removeCard(char rank, char suit);

    // Prints the current cards in the deck to the console
    void printShoe() const;

    int getHiLoCount();
    int getHiLoTrueCount();

    double getPTen(); // density of ten value cards in shoe

    std::vector<Card> getCardsVector();
};