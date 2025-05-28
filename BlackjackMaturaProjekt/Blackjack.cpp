#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

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
    char getRank() const noexcept { return rank; }
    char getSuit() const noexcept { return suit; }
};

class Hand {
    std::vector<Card> cards; // cards in hand
    double bet = 0; // bet is specific to each hand, not player
    bool flagIsInTheGame = true; // is the hand still in the game and therefore needs to be prompted for actions?
    bool flagIsHandResolved = false; // has the hand already been resolved? If not, we need to resolve it at the end of the game.
public:
    void addCard(const Card& card) {
        cards.push_back(card); // adds card to hand
    }

    Card removeCard(int i) {
        if (i < 0 || i >= cards.size()) {
            throw std::out_of_range("Index out of range");
        }
        Card removed = cards[i];
        cards.erase(cards.begin() + i);
        return removed;
    }

    void clear() noexcept {
        cards.clear();
    }

    bool getFlagIsInTheGame() const noexcept { return flagIsInTheGame; }
    void setFlagIsInTheGame(bool flag) { flagIsInTheGame = flag; }
    bool getFlagIsHandResolved() const noexcept { return flagIsHandResolved; }
    void setFlagIsHandResolved(bool flag) { flagIsHandResolved = flag; }

    double getBet() const noexcept {
        return bet;
    }

    void setBet(int bet) noexcept {
        this->bet = bet;
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

    Card getCard(int i = 0) const { // returns card for given index i
        return cards[i];
    }

    int getCardsCount() {
        return cards.size();
    }

    void printHand() const noexcept {
        for (const Card& card : cards) {
            std::cout << card.getRank() << card.getSuit() << " ";
        }
        std::cout << "(" << this->getValue()<<")" << std::endl;
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
    std::unordered_map<std::string, int> sideBets; // dictionary for the sidebet and respective bet amount. includes but not limited to insurance, even money
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
    
    Hand& addHand() {
        hands.emplace_back(); // automatically creates a new hand object
        return hands.back();
    }

    void setSideBet(const std::string& betName, int amount) {
        sideBets[betName] = amount;
    }

    int getSideBet(const std::string& betName) const {
        auto it = sideBets.find(betName);
        return it != sideBets.end() ? it->second : 0; // returns 0 if not set
    }

    std::string getName() const noexcept { return name; }

    int handCount() const noexcept {
        return static_cast<int>(hands.size());
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

    Card getWholeCard() const {
        return hand.getCard(1);
    }

    Card getUpCard() const {
        return hand.getCard(0);
    }

    void printStartingHand() { // prints the dealers hand in the format: UP_CARD; WHOLE_CARD
        std::cout << hand.getCard(0).toString()<<" " << "WHOLECARD" << "(" << hand.getCard(0).getValue() << ")" << std::endl;
    }
    // Access dealer's hand
    Hand& getHand() noexcept { return hand; }
};

struct GameRules {
    int deckCount = 1;
    int maxSplitHands = -1; // infinite
    bool doubleAfterSplit = true;

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
    Dealer d;
    Shoe shoe(1);
    shoe.shuffle();

    // ask for bet(s) and set via setBet
    for (Player& player : players) {
        int bet;
        while (true) {
            std::cout << player.getName() << " Bet: ";
            std::cin >> bet;
            if (std::cin.fail()) {
                std::cin.clear(); // Clear error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                std::cout << "Invalid input! Please enter a number.\n";
            }
            else {
                break; // Valid number entered
            }
        }
        for (Hand& hand : player.getHands()) {
            hand.setBet(bet);
        }
    }

    // first all players get 1 card, then dealer gets 1 card, then all players second card, then dealer second card(hole card which is hidden)
    for (int i = 0; i < players.size(); ++i) { // add 2 cards to all players, then dealer.

        players[i].addCard(Card('T','S')); // removes from shoe, adds to player
    }

    d.addCard(Card('T', 'S'));
    for (int i = 0; i < players.size(); ++i) { // add 2 cards to all players, then dealer.

        players[i].addCard(Card('T', 'S')); // removes from shoe, adds to player
    }
    d.addCard(shoe.deal());

    // start with pregame checks now
    // 
   // check if dealer has ace
    // if he does:
        // offer all players insurance, those who have blackjack are offered even money
        // dealer peeks whole card, resolves sidebets. If dealer has blackjack, end round.
    // if he doesnt, check if he has 10
    // if he does:
        // peek whole card, end round if blackjack
    // pregame checks over, continue with normal game if not aborted

    bool flagRoundEnd = false;

    if (d.getUpCard().getRank() == 'A' and not flagRoundEnd) { // dealer has ace upcard, offer insurance and if necessary even money

        for (int i = 0; i < players.size(); ++i) { // loop through every player
            char choice = 'f';
            if (players[i].getHands()[0].getValue() == 21) { // players has blackjack, offer even money

                while (choice != 'y' && choice != 'n') {
                    std::cout << "Do you want even money(y/n): ";
                    std::cin >> choice;
                    if (choice != 'y' && choice != 'n') {
                        std::cout << "Invalid, please enter a valid choice!\n";
                    }
                }
                
                if (choice == 'y') {
                    // give 1:1 payout immediately!
                    choice = 'f';
                }
                else if (choice == 'n') {
                    // dont do anything
                    choice = 'f';
                }
            }
            else { // player doesnt have blackjack, offer insurance
                while (choice != 'y' && choice != 'n') {
                    std::cout << "Do you want insurance(y/n): ";
                    std::cin >> choice;
                    if (choice != 'y' && choice != 'n') {
                        std::cout << "Invalid, please enter a valid choice!\n";
                    }
                }
                if (choice == 'y') {
                    // set insurance sidebet
                    players[i].setSideBet("insurance", players[i].getHands()[0].getBet() / 2);

                }else if (choice == 'n') {
                    // dont do anything
                }

            }
            
        }
        // offered everything to all players, dealer peeks whole card now
        if (d.getWholeCard().getValue() == 10) { // Ass up card + 10 hidden card = 21. 
            // dealer blackjack, end round!
        }

    }
    else if (d.getUpCard().getValue() == 10 and not flagRoundEnd) { // game not ended yet, continue with checks

        // check for dealer blackjack and dont offer any sidebets. If dealer has 10+A round ends immedisately always.
        flagRoundEnd = true;
    }

    // pregame checks over

    // loop through all hands that havent been resolved yet, ask the owner of the hand what to do.
    for (Player& player : players) { // loop all players
        for (size_t i = 0; i < player.getHands().size(); ++i) { // loop all hands of a player
            // check and perform a hand.
            
            if (player.getHands()[i].getFlagIsInTheGame()) {
                // hand still in the game
                // ask the owner what to do
                std::cout << "Dealer: ";
                d.printStartingHand();
                std::cout << "\n"<<player.getName()<<": ";
                player.getHands()[i].printHand();
                
                // determine which actions are allowed, prompt the player with the allowed actions, reject any non-legal action
                //hitting always allowed, standing always allowed, double allowed based on rules, splitting allowed when same pair AND rules
                std::vector<char> allowedChoices;
                allowedChoices.push_back('h'); allowedChoices.push_back('s'); // these are always allowed
                // double allowed anytime on the first 2 cards of a hand. SO if a hand has exactly 2 cards, double down is allowed
                if (player.getHands()[i].getCardsCount() == 2) {
                    // hand has exactly 2 cards, double is allowed.
                    allowedChoices.push_back('d');
                    // also if we are on two cards, check if cards are the same, splitting would also be allowed
                    if (player.getHands()[i].getCard(0).getRank() == player.getHands()[i].getCard(1).getRank()) {
                        // splitting allowed
                        allowedChoices.push_back('p');
                    }
                }
                // legal actions determined and safed in allowedChoices vector
                char choice = 'f';
                while (std::find(allowedChoices.begin(), allowedChoices.end(), choice) == allowedChoices.end()) {
                    std::cout << "Available actions: ";
                    for (char action : allowedChoices) {
                        switch (action) {
                        case 'h': std::cout << "Hit (h) "; break;
                        case 's': std::cout << "Stand (s) "; break;
                        case 'd': std::cout << "Double (d) "; break;
                        case 'p': std::cout << "Split (p) "; break;

                        default: std::cout << action << " "; break;
                        }
                    }
                    std::cout << std::endl << "Choice: ";

                    std::cin >> choice;
                    if (std::find(allowedChoices.begin(), allowedChoices.end(), choice) == allowedChoices.end()) {
                        std::cout << "Invalid choice!" << std::endl;
                    }
                }
                // we now have a legal choice. Execute that choice now.
                if (choice == 'h') {
                    // add card to hand
                    player.getHands()[i].addCard(shoe.deal());
                }
                else if (choice == 's') {
                    // change flagInTheGame
                    player.getHands()[i].setFlagIsInTheGame(false);
                }
                else if (choice == 'd') {
                    // double bet and change flagInTheGame
                    player.getHands()[i].setBet(player.getHands()[i].getBet() * 2);
                    player.getHands()[i].setFlagIsInTheGame(false);
                }
                else if (choice == 'p') {
                    // remove the second card of the hand, create a new hand object and add that card, then deal 2 new cards
                    std::cout << player.getHands()[i].getCardsCount();
                    player.addHand(); // we have hand and hand2 to work with
                    
                    player.getHands()[i+1].addCard(player.getHands()[i].removeCard(1)); // remove it and add it to the other hand, then redeal new cards to those hands
                    player.getHands()[i].addCard(shoe.deal());
                    player.getHands()[i+1].addCard(shoe.deal());
                    player.getHands()[i+1].setBet(player.getHands()[i].getBet()); // match the bet of the new hand to the old one
                    i--; // reduce i by 1 after splitting because we want to make another choice for both new hands
                }
            }
        }
    }

    return 0;
}
