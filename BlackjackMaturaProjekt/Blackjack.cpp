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
    bool flagIsHandResolved = false; // has the hand already been resolved? If not, we need to resolve it at the end of the game. Important for surrender
    bool flagEvenMoney = false; // did the player take even money for this hand
    bool flagSurrendered = false; // did the player surrender the hand
    bool flagIsSplitHand = false; // is this hand the result of splitting a previous hand
    std::unordered_map<std::string, double> sideBets; // dictionary for the sidebet and respective bet amount. includes but not limited to insurance, even money
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
    void setFlagEvenMoney(bool flag) { flagEvenMoney = flag; }
    bool getFlagEvenMoney() const noexcept { return flagEvenMoney; }
    void setFlagSurrendered(bool flag) { flagSurrendered = flag; }
    bool getFlagSurrendered() const noexcept { return flagSurrendered; }
    void setFlagIsSplitHand(bool flag) { flagIsSplitHand = flag; }
    bool getFlagIsSplitHand() const noexcept { return flagIsSplitHand; }

    void setSideBet(const std::string& betName, double amount) {
        sideBets[betName] = amount;
    }

    double getSideBet(const std::string& betName) const {
        auto it = sideBets.find(betName);
        return it != sideBets.end() ? it->second : 0; // returns 0 if not set
    }

    double getBet() const noexcept {
        return bet;
    }

    void setBet(double bet) noexcept {
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

    bool isSoft() { // determines if hand is soft(contains aces counted as 11)
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
        if (softAceCount > 0) {
            return true;
        }
        return false;
    }

    Card getCard(int i = 0) const { // returns card for given index i
        return cards[i];
    }

    int getCardsCount() {
        return static_cast<int>( cards.size());
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
    int deckSize;
public:
    Shoe(int size = 1) { // constructor initializes shoe with 52 cards or multiple of 52 if specified.
        deckSize = size;
        reset();
    }

    // Shuffles deck using random number generator
    void shuffle() {
        std::random_device rd;              // Random seed
        std::mt19937 g(rd());               // Random number generator
        std::shuffle(cards.begin(), cards.end(), g); // Shuffle the deck
    }

    void reset() {
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


    Card deal() {
        if (cards.empty()) {
            reset();
            std::cout << "Shuffeled Deck!";
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
    double balance = 10;
public:
    // Constructor to set player name and initialize one hand
    Player(const std::string& playerName) : name(playerName) {
        hands.emplace_back();
    }

    void setBalance(double balance) {
        this->balance = balance;
    }
    
    double getBalance() const noexcept{ return balance; } 
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
    int maxSplitHands = 1000000; // infinite
    double blackjackPayoutRatio = 3 / 2;
    bool doubleAfterSplit = true;
    bool soft17 = false; // false means h17
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = true;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = true;
    bool hitSplitAcesAllowed = true;
    bool doubleDownAlwaysAllowed = true; 
    std::vector<int> doubleDownOnCardValues; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
};

int main()
{
    std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::vector<Player> players;
    GameRules r;
    Player p("Nevio");
    players.push_back(p);
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();

    // ask for bet(s) and set via setBet
    for (Player& player : players) {
        for (size_t i = 0; i < player.getHands().size(); ++i) { // loop all hands
            Hand& hand = player.getHands()[i];
            double bet;
            while (true) {
                std::cout << player.getName() <<"'s hand " <<i+1 << " Bet: ";
                std::cin >> bet;
                if (std::cin.fail()) {
                    std::cin.clear(); // Clear error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    std::cout << "Invalid input! Please enter a number.\n";
                }
                else if (bet < r.minBet) {
                    std::cin.clear(); // Clear error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    std::cout << "Invalid input! Please enter a number greater than "<<r.minBet<<std::endl;
                }
                else if (bet > r.maxBet) {
                    std::cin.clear(); // Clear error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    std::cout << "Invalid input! Please enter a number smaller than " << r.maxBet << std::endl;
                }
                else if (player.getBalance()<bet) {
                    std::cin.clear(); // Clear error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    std::cout << "Balance too low, please lower your bet!"; 
                    std::cout << " Your balance is: " << player.getBalance()<<std::endl;
                }
                else {
                    break; // Valid number entered
                }
            }
                hand.setBet(bet); // set the hands bet
                player.setBalance(player.getBalance() - bet); // remove bet from balance
        }
    }

    // first all players get 1 card, then dealer gets 1 card, then all players second card, then dealer second card(hole card which is hidden)
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            hand.addCard(shoe.deal());
        }
    }

    d.addCard(shoe.deal());
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            hand.addCard(shoe.deal());
        }
    }
    d.addCard(shoe.deal());

    // before we start with pregame checks, offer early surrender if allowed

    if (r.earlySurrenderAllowed) {
        for (Player& player : players) {
            for (size_t i = 0; i < player.getHands().size(); ++i) {
                char choice = 'f';
                // ask every player if they want to surrender any one of their hands
                // print situation first (dealer hand and then player hand)
                std::cout << "\nDealer: ";
                d.printStartingHand(); std::cout << std::endl;
                std::cout << player.getName() << "'s balance: " << player.getBalance() << std::endl;
                std::cout << "" << player.getName() << "'s hand " << i + 1 << ": ";
                player.getHands()[i].printHand();
                // after printing situation, ask if he wants to surrender the hand
                while (choice != 'y' && choice != 'n') {
                    std::cout << "Do you want to surrender(y/n): ";
                    std::cin >> choice;
                    if (choice != 'y' && choice != 'n') {
                        std::cout << "Invalid, please enter a valid choice!\n";
                    }
                }

                if (choice == 'y') {
                    // even money was taken, set flag for later use in determining payout
                    player.getHands()[i].setFlagSurrendered(true);
                    player.getHands()[i].setFlagIsInTheGame(false);
                    choice = 'f';
                }
                else if (choice == 'n') {
                    // dont do anything
                    choice = 'f';
                }
            }
        }
    }

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

    if (d.getUpCard().getRank() == 'A' and not flagRoundEnd and r.peakAmericanStyle) { 
        // dealer has ace upcard, offer insurance and if necessary even money
        for (Player& player : players) { // loop through every player
            for (Hand& hand : player.getHands()) {
                char choice = 'f';
                if (hand.getValue() == 21 && r.evenMoneyAllowed) { // player has blackjack, offer even money and disable the hand
                    hand.setFlagIsInTheGame(false);
                    while (choice != 'y' && choice != 'n') {
                        std::cout << "Do you want even money(y/n): ";
                        std::cin >> choice;
                        if (choice != 'y' && choice != 'n') {
                            std::cout << "Invalid, please enter a valid choice!\n";
                        }
                    }

                    if (choice == 'y') {
                        // even money was taken, set flag for later use in determining payout
                        hand.setFlagEvenMoney(true);
                        choice = 'f';
                    }
                    else if (choice == 'n') {
                        // dont do anything
                        choice = 'f';
                    }
                }
                else { // player doesnt have blackjack, offer insurance(sidebet)
                    while (choice != 'y' && choice != 'n' && r.insuranceAllowed) {
                        std::cout << "Do you want insurance(y/n): ";
                        std::cin >> choice;
                        if (choice != 'y' && choice != 'n') {
                            std::cout << "Invalid, please enter a valid choice!\n";
                        }
                        else if (choice == 'y' && player.getBalance()<hand.getBet()/2) { // player doesnt have enough money for insurance sidebet
                            std::cout << "Invalid, balance not high enough!";
                            choice = 'f'; // in order to force player to choose n
                        }
                    }
                    if (choice == 'y') {
                        // set insurance sidebet
                        hand.setSideBet("insurance", hand.getBet() / 2);
                        player.setBalance(player.getBalance() - hand.getBet() / 2); // remove appropriate balance from the player

                    }
                    else if (choice == 'n') {
                        // dont do anything
                    }
                }
            }
            
        }
        // offered everything to all players, dealer peeks whole card now
        if (d.getHand().getValue() == 21) { 
            // dealer blackjack, end round!
            flagRoundEnd = true;
        }

    }
    else if (d.getUpCard().getValue() == 10 and not flagRoundEnd and d.getHand().getValue() == 21 && r.peakAmericanStyle) { // game not ended yet, continue with checks

        // check for dealer blackjack and dont offer any sidebets. If dealer has 10+A round ends immediately always.
        flagRoundEnd = true;
    }

    // pregame checks over
    

    if (flagRoundEnd) {
        for (Player& player : players) {
            for (Hand& hand : player.getHands()) {
                hand.setFlagIsInTheGame(false); // since round was ended, remove all hands from the game and wrap up the game instantly
            }
        }
    }

    // after the dealer has checked for blackjack, offer late surrender if allowed to all hands that are still in the game

    // loop through all hands that havent been played yet, ask the owner of the hand what to do.
    for (Player& player : players) { // loop all players
        for (size_t i = 0; i < player.getHands().size(); ++i) { // loop all hands of a player
            // check and perform a hand.
            int j = 0;
            while (player.getHands()[i].getFlagIsInTheGame()) {
                j += 1;
                // hand still in the game
                // ask the owner what to do

                // start by asking if he wants to surrender the hand
                if (r.lateSurrenderAllowed && j == 1) { // dont reoffer surrender if we are not on first iteration
                    char choice = 'f';
                    // ask every player if they want to surrender any one of their hands
                    // print situation first (dealer hand and then player hand)
                    std::cout << "\nDealer: ";
                    d.printStartingHand(); std::cout << std::endl;
                    std::cout << player.getName() << "'s balance: " << player.getBalance() << std::endl;
                    std::cout << "" << player.getName() << "'s hand " << i + 1 << ": ";
                    player.getHands()[i].printHand();
                    // after printing situation, ask if he wants to surrender the hand
                    while (choice != 'y' && choice != 'n') {
                        std::cout << "Do you want to surrender(y/n): ";
                        std::cin >> choice;
                        if (choice != 'y' && choice != 'n') {
                            std::cout << "Invalid, please enter a valid choice!\n";
                        }
                    }

                    if (choice == 'y') {
                        // even money was taken, set flag for later use in determining payout
                        player.getHands()[i].setFlagSurrendered(true);
                        player.getHands()[i].setFlagIsInTheGame(false);
                        choice = 'f';
                        break;
                    }
                    else if (choice == 'n') {
                        // dont do anything
                        choice = 'f';
                    }
                }

                std::cout << "\nDealer: ";
                d.printStartingHand(); std::cout << std::endl;
                std::cout << player.getName() << "'s balance: " << player.getBalance()<<std::endl;
                std::cout << ""<<player.getName()<<"'s hand "<<i+1<<": ";
                player.getHands()[i].printHand();
                
                
                // determine which actions are allowed, prompt the player with the allowed actions, reject any non-legal action
                //hitting always allowed, standing always allowed, double allowed based on rules, splitting allowed when same pair AND rules
                std::vector<char> allowedChoices;
                allowedChoices.push_back('s'); // these are always allowed
                if (not(player.getHands()[i].getFlagIsSplitHand() and player.getHands()[i].getCard(0).getRank() == 'A' and not r.hitSplitAcesAllowed)) {
                    // the hand is a split resulting from a previous hand with 2 aces, only allow hitting when rules allow it
                    allowedChoices.push_back('h');
                }
                // double allowed anytime on the first 2 cards of a hand. SO if a hand has exactly 2 cards, double down is allowed
                // additional check to ensure player has the balance necessary to double down
                // additional check with rules
                if (player.getHands()[i].getCardsCount() == 2 && player.getBalance()>=player.getHands()[i].getBet()) {

                    if (r.doubleDownAlwaysAllowed) {
                        if (r.doubleAfterSplit) {
                            // always allowed
                            allowedChoices.push_back('d');
                        }
                        else if (not player.getHands()[i].getFlagIsSplitHand()) {
                            // only allowed when the hand is NOT a split hand
                            allowedChoices.push_back('d');
                        }
                    }
                    else if(std::find(r.doubleDownOnCardValues.begin(), r.doubleDownOnCardValues.end(), 
                        player.getHands()[i].getValue()) != r.doubleDownOnCardValues.end()){ // searches the vector for the current hand value
                        // if doubling is not always allowed, check if the current value is in the array of allowed double values.
                        if (r.doubleAfterSplit) {
                            // always allowed
                            allowedChoices.push_back('d');
                        }
                        else if (not player.getHands()[i].getFlagIsSplitHand()) {
                            // only allowed when the hand is NOT a split hand
                            allowedChoices.push_back('d');
                        }
                    }
                    // also if we are on two cards, if cards are the same, splitting would also be allowed
                    // check that the amount of hands the player has does not surpas the max split limit
                    // if resplitting aces is not allowed, prohibit splitting when both cards are aces and the hand has flag isSplitHand
                    if (player.getHands()[i].getCard(0).getRank() == player.getHands()[i].getCard(1).getRank() 
                        and player.getHands().size()<=r.maxSplitHands) {
                        if (not(player.getHands()[i].getCard(0).getRank() == 'A' and player.getHands()[i].getFlagIsSplitHand() and not r.resplitAcesAllowed)) {
                            // only when both cards are Aces and the hand is a split hand already and resplitting aces is prohibited will this not run
                            allowedChoices.push_back('p');
                        }
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
                    // double bet, add card and change flagInTheGame
                    player.getHands()[i].setBet(player.getHands()[i].getBet() * 2);
                    player.getHands()[i].addCard(shoe.deal());
                    player.setBalance(player.getBalance() - player.getHands()[i].getBet() / 2);
                    player.getHands()[i].setFlagIsInTheGame(false);
                    std::cout << player.getName() << "'s hand " << i + 1 << ": ";
                    player.getHands()[i].printHand();
                    std::cout << player.getName() << "'s balance: " << player.getBalance()<<std::endl;
                }
                else if (choice == 'p') {
                    // remove the second card of the hand, create a new hand object and add that card, then deal 2 new cards
                    player.addHand(); // we have hand and hand2 to work with
                    
                    player.getHands()[i].setFlagIsSplitHand(true);
                    player.getHands()[i+1].setFlagIsSplitHand(true);

                    player.getHands()[i+1].addCard(player.getHands()[i].removeCard(1)); // remove it and add it to the other hand, then redeal new cards to those hands
                    player.getHands()[i].addCard(shoe.deal());
                    player.getHands()[i+1].addCard(shoe.deal());
                    player.getHands()[i+1].setBet(player.getHands()[i].getBet()); // match the bet of the new hand to the old one
                    player.setBalance(player.getBalance() - player.getHands()[i].getBet()); 
                    
                }
                // after the choice is executed, check if we busted or not, if we did we need to disable the hand 
                if (player.getHands()[i].getValue() > 21) { // only > not >= because you could still technically hit a hand with value 21 (except on starting hand)
                    player.getHands()[i].setFlagIsInTheGame(false);
                    std::cout << "--BUSTED!-- \n";
                    std::cout << player.getName() << "'s hand " << i + 1 << ": ";
                    player.getHands()[i].printHand();
                }
            }
        }
    }
    // after all player hands have been played out, the dealer needs to play out his hand
    // dealer first reveals wholecard (print hand function without hiding) and then hits until he reaches a value of greater than 17
    std::cout << "\n--Dealer turn--\n";
    std::cout << "Dealer: ";  d.getHand().printHand();
    while (d.getHand().getValue() < 17 or (d.getHand().getValue() == 17 and d.getHand().isSoft() and not r.soft17)) { // soft17 = false means h17 rules so hitting
        // as long as dealer is below 16 OR if on 17 AND soft he must hit(depending on rules)
        d.getHand().addCard(shoe.deal());
        std::cout << "Dealer hit hand: "; d.getHand().printHand();
    }
    std::cout << std::endl;
    // dealer hand determined
    // all hands have been played, start evaluating results now
    // // after setting the bet, the wagered amount is instantly subtracted from the balance. THe idea being that its no longer in the account, its on the table
    // if he loses, he loses the bet. 
    // if he pushes, bet is returned
    // if he wins, he gets his bet back twice
    // if he has blackjack with 2 cards(starting blackjack) he gets his bet back plus a special payout ratio determined in the gamerules
    for (Player& player : players) {
        for (size_t i = 0; i < player.getHands().size(); ++i) { // loop all hands
            // loop all hands without exception and determine the payout based on dealer hand and player hand
            // also include sidebet payouts
            Hand& hand = player.getHands()[i];
            // if we busted we always lose
            // if we didnt bust: If we have higher value than dealer we win, if we have same we push, if we dont we lose
            // special: If we have a 21 and exactly 2 cards we have a blackjack: return base bet + Payout 3:2 or 1:1 if even moeny was taken.

            if (d.getHand().getValue() == 21 and d.getHand().getCardsCount() == 2 && player.getHands()[i].getSideBet("insurance")) {
                // dealer has blackjack, payout insurance sidebets
                // only runs if insurance sidebet was greater than 0
                double payout = hand.getSideBet("insurance") * 2; // this is only the insurance payout, real hand comes later
                std::cout << "Dealer had blackjack, " << player.getName() << "'s hand " << i + 1 << " insurance payout: " << payout<<"\n";
                player.setBalance(player.getBalance() + payout);
                
            }
            // sidebet resolution over

            if (hand.getFlagSurrendered()) { // we surrendered the hand, return bet/2
                double payout = hand.getBet() / 2;
                std::cout << player.getName() << "'s hand " << i + 1 << " had surrendered, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
            } 
            else if (hand.getValue() > 21) {
                // we lose
                double payout = 0;
                std::cout << player.getName() << "'s hand " << i + 1 << " lost, payout: " << payout<<"\n";
                
            }
            else if (hand.getValue() == 21 and hand.getCardsCount() == 2 and player.getHands().size() == 1) { // this is a true blackjack
                if (hand.getFlagEvenMoney()) {
                    // we have true blackjack with even money, return bet twice because of the 1:1 payout
                    double payout = hand.getBet()*2;
                    std::cout << player.getName() << "'s hand " << i + 1 << " had even money, payout: " << payout << "\n";
                    player.setBalance(player.getBalance() + payout);
                }
                else {
                    // we have true blackjack without even money, compare with dealer value to determine payout(push or blackjack)
                    if (d.getHand().getValue() == 21) {
                        // push
                        double payout = hand.getBet();
                        std::cout << player.getName() << "'s hand " << i + 1 << " pushes blackjack, payout: " << payout << "\n";
                        player.setBalance(player.getBalance() + payout);
                    }
                    else {
                        // BLACKJACK
                        double payout = hand.getBet() +hand.getBet()*r.blackjackPayoutRatio;
                        std::cout << player.getName() << "'s hand " << i + 1 << " wins blackjack, payout: " << payout << "\n";
                        player.setBalance(player.getBalance() + payout);
                    }
                }
            }
            else if (hand.getValue() > d.getHand().getValue()) {
                // win
                double payout = hand.getBet()*2;
                std::cout << player.getName() << "'s hand " << i + 1 << " wins, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
            }
            else if (hand.getValue() == d.getHand().getValue()) {
                // push
                double payout = hand.getBet();
                std::cout << player.getName() << "'s hand " << i + 1 << " pushes, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
            }
            else if (hand.getValue() < d.getHand().getValue()) {
                // lose
                double payout = 0;
                std::cout << player.getName() << "'s hand " << i + 1 << " loses, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
            }
        }
        std::cout << player.getName() << "'s new balance: " << player.getBalance() << "\n";
    }
    return 0;
}
