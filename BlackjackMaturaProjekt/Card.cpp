#include "Card.h"
// Constructor initializes the rank and suit of the card
Card::Card(char r, char s) : rank(r), suit(s) {}

// Returns the string representation of the card (e.g., "AS" for Ace of Spades)
std::string Card::toString() const {
    return std::string(1, rank) + suit;
}

// Returns the blackjack value of the card
int Card::getValue() const {
    if (rank >= '2' && rank <= '9') { // Numeric cards (2-9) are valued as their rank
        return rank - '0'; // works because of ascii values
    }
    else if (rank == 'T' || rank == 'J' || rank == 'Q' || rank == 'K') { // Face cards and 10 are valued as 10
        return 10;
    }
    else if (rank == 'A') { // Ace is valued as 11 by default
        return 11;
    }
    return 0; // Invalid rank; should not occur with proper card initialization
}

// Returns the rank of the card
char Card::getRank() const noexcept { return rank; }
char Card::getSuit() const noexcept { return suit; }