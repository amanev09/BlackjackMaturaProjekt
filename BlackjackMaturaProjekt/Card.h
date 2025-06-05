#pragma once
#include <string>

class Card {
private:
    char rank;  // Represents the rank of the card ('2'-'9', 'T', 'J', 'Q', 'K', 'A')
    char suit;  // Represents the suit of the card ('S' (Spades), 'H' (Hearts), 'D' (Diamonds), 'C' (Clubs))
public:
    // Constructor initializes the rank and suit of the card
    Card(char r, char s);

    // Returns the string representation of the card (e.g., "AS" for Ace of Spades)
    std::string toString() const;

    // Returns the blackjack value of the card
    int getValue() const;

    // Returns the rank of the card
    char getRank() const noexcept;
    char getSuit() const noexcept;
};