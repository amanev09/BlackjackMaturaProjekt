#include <iostream>
#include <vector>
#include <random>

class Card {
private:
    char rank;  // Represents the rank of the card ('2'-'9', 'T', 'J', 'Q', 'K', 'A')
    char suit;  // Represents the suit of the card ('S' (Spades), 'H' (Hearts), 'D' (Diamonds), 'C' (Clubs))
public:
    // Constructor initializes the rank and suit of the card
    Card(char r, char s) : rank(r), suit(s) {}

    // Returns the string representation of the card (e.g., "AS" for Ace of Spades)
    std::string toString() const {
        return std::string(1, rank) + suit;
    }

    // Returns the blackjack value of the card
    int getValue() const {
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
    char getRank() const noexcept {
        return rank;
    }
    char getSuit() const noexcept {
        return suit;
    }
};

class Hand {
    std::vector<Card> cards; // cards in hand
    double bet = 0; // bet is specific to each hand, not player
public:
    void addCard(const Card& card) {
        cards.push_back(card); // adds card to hand
    }

    void clear() noexcept {
        cards.clear();
    }

    int getValue() const noexcept { // returns value of all Hands adjusted for Aces
        int totalValue = 0;
        int softAceCount = 0;

        for (const Card& card : cards) {
            int cardValue = card.getValue();
            totalValue += cardValue;
            if (cardValue == 11) {
                softAceCount++;
            }
        }
        while (totalValue > 21 && softAceCount > 0) {
            // treat soft aces hard until below 21 or no more soft aces in hand
            totalValue -= 10;
            softAceCount--;
        }
        return totalValue;
    }

    void printHand() const noexcept {
        for (const Card& card : cards) {
            std::cout << card.getRank() << card.getSuit() << " ";
        }
        std::cout << std::endl;
    }
};

class Shoe {
    std::vector<Card> cards; // Holds all cards in shoe
public:
    Shoe(int size = 1) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
        std::string ranks = "23456789TJQKA";
        std::string suits = "SHDC";
        for (int i = 0; i < size; ++i) {
            for (char suit : suits) {
                for (char rank : ranks) {
                    cards.emplace_back(rank, suit); // automatically constructs card and adds to shoe.
                }
            }
        }
    }

    // Shuffles deck using random number generator
    void shuffle() {
        std::random_device rd;              // Random seed
        std::mt19937 g(rd());               // Random number generator
        std::shuffle(cards.begin(), cards.end(), g); // Shuffle the deck
    }

    Card deal() {
        if (cards.empty()) {
            throw std::out_of_range("No cards left in deck");
        }
        Card dealtCard = cards.back();
        cards.pop_back();
        return dealtCard;
    }

    // Prints the current cards in the deck to the console
    void printDeck() const {
        for (const auto& card : cards) {
            std::cout << card.toString() << " ";
        }
        std::cout << std::endl;
    }
};

class Player {
    std::string name;
    std::vector<Hand> hands;
    int bet = 0;
public:
    // Constructor to set player name and initialize one hand
    Player(const std::string& playerName) : name(playerName) {
        hands.emplace_back();
    }

    // Add a card to the specified hand (default: first hand)
    void addCard(const Card& card, size_t handIndex = 0) {
        if (handIndex < hands.size()) {
            hands[handIndex].addCard(card);
        }
    }
    
    int handCount() const {
        return hands.size();
    }

    int getBet() const {
        return bet;
    }

    void setBet(int bet) {
        this->bet = bet;
    }

    // Access player's hands
    std::vector<Hand>& getHands() noexcept { return hands; }
    const std::vector<Hand>& getHands() const noexcept { return hands; }

};

class Dealer {
    Hand hand; // second entry is hole card
public:
    // Prepare dealer for new round
    void clearHand() noexcept {
        hand.clear();
    }

    // Add card to dealer's hand
    void addCard(const Card& card) {
        hand.addCard(card);
    }

    // Access dealer's hand
    Hand& getHand() noexcept { return hand; }
};

struct GameRules {
    int deckCount = 1;
};



void blackjackRound() { // plays one full round of blackjack for given arguments.

}

int main()
{
    std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::vector<Player> players;
    GameRules r;
    Player p("Nevio");
    players.push_back(p);
    p.setBet(1);
    Dealer d; 
    Shoe shoe(1);
    shoe.shuffle();

    
    // first all players get 1 card, then dealer gets 1 card, then all players second card, then dealer second card(hole card which is hidden)
    for (int i = 0; i < players.size(); ++i) { // add 2 cards to all players, then dealer.

        players[i].addCard(shoe.deal()); // removes from shoe, adds to player
        players[i].getHands()[0].printHand();
    }
    
    d.addCard(shoe.deal());
    for (int i = 0; i < players.size(); ++i) { // add 2 cards to all players, then dealer.

        players[i].addCard(shoe.deal()); // removes from shoe, adds to player
        players[i].getHands()[0].printHand();
    }
    d.addCard(shoe.deal());
    
    // start with pregame checks now
    // 
    // first check if dealer has an ace
    // if he does, offer all players insurance, except the ones who have a blackjack, offer them even money
    // dealer peeks whole card, resolve sidebets. If dealer has blackjack, end round.
    // then check if dealer has a 10. If he does, also end the round.

    
    std::cout << "Hell\n";


    return 0;
}
