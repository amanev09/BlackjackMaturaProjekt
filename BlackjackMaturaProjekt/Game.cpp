#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"

gameResult blackJack(GameRules& r, std::vector<Player>& players, Dealer& d, Shoe& shoe, const std::unordered_map<std::string, std::string>* inputChoices,
    std::unordered_map<std::string, Card>* startingCards,
    const std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* cache) {
    // plays one round of blackjack using certain rules for all players on the table
    gameResult gr; // for statistican analysis, tracks the result for all players and their hands


    auto getInput = [&](const std::string& promptKey, const std::string& cacheKey = "-1", const std::vector<char> allowedChoices = {}) -> std::string {

        if (cache && cache->count(cacheKey)) {
            const auto& vec = cache->at(cacheKey);

            if (promptKey == "choice") {
                for (const auto& pair : vec) {
                    const auto& map = pair.second;
                    for (char c : allowedChoices) {
                        if (map.count("choice") && map.at("choice") == std::string(1, c)) {
                            std::cout << map.at("choice")<<"\n";
                            return map.at("choice");
                        }
                    }
                }
            }

            if (!vec.empty()) {
                const auto& innerMap = vec[0].second;
                if (innerMap.count(promptKey)) {
                    return innerMap.at(promptKey);
                }
            }
        }
        if (inputChoices && inputChoices->count(promptKey)) {
            if (promptKey == "choice") {
                for (char c : allowedChoices) {
                    if (inputChoices->at(promptKey) == std::string(1, c)) {
                        std::cout << inputChoices->at(promptKey) << std::endl;
                        return inputChoices->at(promptKey);
                    }
                }
                // occurs when we split A,A and get A,A and choice is 'p' but gamerules prohibit resplitting aces
                return std::string(1, allowedChoices[0]);
            }
            std::cout << inputChoices->at(promptKey)<<std::endl;
            return inputChoices->at(promptKey);
        }
        std::string choice;
        std::cin >> choice;
        return choice;
        };

    auto getStarterCard = [&](const std::string& promptKey) -> Card {
        if (startingCards && startingCards->count(promptKey)) {
            Card c = startingCards->at(promptKey);
            
            return c;
        }
        else {
            return shoe.deal(); // if we dont have a card assigned, just deal like normal
        }
        };

    // ask every player how many to play with
    for (Player& player : players) {
        playerResult pr;
        gr.playerResults[player.getName()] = pr; // stores all player results in here with key as the player name
        
        int amount; 

        while (true) {
            std::cout << "How many hands do you want to play with (max " << r.maxSplitHands << "): ";
            std::string input = getInput("numHands");
            try {
                amount = std::stoi(input);

                if (amount <= 0 || amount > r.maxSplitHands) {
                    std::cout << "Invalid input! Please enter an integer between 1 and " << r.maxSplitHands << ".\n";
                }
                else {
                    break; // success
                }

            }
            catch (const std::invalid_argument&) {
                std::cout << "Invalid input! Please enter a valid number.\n";
            }
            catch (const std::out_of_range&) {
                std::cout << "Number out of range! Try a smaller number.\n";
            }
        }

        // setup each player with the requested amount hands according to the amount variable
        for (int i = 0; i < amount; ++i) {
            player.addHand();
            gr.playerResults[player.getName()].handResults.emplace_back(); // for every pr object(1 per player) create as many hand result structs as the player starts with.
        }
    }

    // ask for bet(s) and set via setBet
    for (Player& player : players) {
        for (size_t i = 0; i < player.getHands().size(); ++i) { // loop all hands
            Hand& hand = player.getHands()[i];
            double bet;
            while (true) {
                std::cout << player.getName() << "'s hand " << i + 1 << " Bet: ";
                std::string input = getInput("bet");
                try {
                    bet = std::stod(input);

                    if (bet < r.minBet) {
                        std::cout << "Invalid input! Please enter a number greater than " << r.minBet << std::endl;
                    }
                    else if (bet > r.maxBet) {
                        std::cout << "Invalid input! Please enter a number smaller than " << r.maxBet << std::endl;
                    }
                    else if (player.getBalance() < bet) {
                        std::cout << "Balance too low, please lower your bet!";
                        std::cout << " Your balance is: " << player.getBalance() << std::endl;
                    }
                    else {
                        break; // valid input
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Invalid input! Please enter a valid number.\n";
                }
                catch (const std::out_of_range&) {
                    std::cout << "Number out of range! Try a smaller number.\n";
                }
            }
            hand.setBet(bet); // set the hands bet
            player.setBalance(player.getBalance() - bet); // remove bet from balance
        }
    }

    // first all players get 1 card, then dealer gets 1 card, then all players second card, (not ENHC) then dealer second card(hole card which is hidden)
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            hand.addCard(getStarterCard("player1"));
        }
    }

    d.addCard(getStarterCard("dealer1"));
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            hand.addCard(getStarterCard("player2"));
        }
    }
    if(r.peakAmericanStyle){ d.addCard(getStarterCard("dealer2")); }
    
    // we have given the players their starting cards
    // optional '3rd' starting card to allow simulations like a multi card 21 or a soft 21 with more than 2 cards(specifically 3 cards)
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            if (startingCards and startingCards->count("player3")) {
                // we pass a third starting card for simulation purposes
                hand.addCard(getStarterCard("player3"));
            }
        }
    }
    // quick print unique game key (identifier)q
    std::string gameKey = makeUniqueKey(players[0].getHands()[0].getValue(), players[0].getHands()[0].getCardsCount(), players[0].getHands()[0].isSoft(), players[0].getHands()[0].isSplittable(), d.getUpCard().getValue(), shoe);
    //std::cout << gameKey;
    // starting from here, we will also pass the 
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
                    std::string input = getInput("earlySurrender", gameKey);
                    choice = input[0];
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
            for (size_t i = 0; i < player.getHands().size(); ++i) {
                Hand& hand = player.getHands()[i];
                char choice = 'f';
                if (hand.getValue() == 21 && r.evenMoneyAllowed && hand.getCardsCount() == 2) { // player has natural blackjack, offer even money and disable the hand
                    hand.setFlagIsInTheGame(false);
                    while (choice != 'y' && choice != 'n') {
                        std::cout << "Do you want even money(y/n): ";

                        std::string input = getInput("evenMoney", gameKey);
                        choice = input[0];

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

                        std::string input = getInput("insurance", gameKey);
                        choice = input[0];

                        if (choice != 'y' && choice != 'n') {
                            std::cout << "Invalid, please enter a valid choice!\n";
                        }
                        else if (choice == 'y' && player.getBalance() < hand.getBet() / 2) { // player doesnt have enough money for insurance sidebet
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
                        gr.playerResults[player.getName()].handResults[i].insuranceOutcome = InsuranceOutcome::NotTaken;
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
    for (Player& player : players) {
        for (Hand& hand : player.getHands()) {
            if (hand.getValue() == 21 and hand.getCardsCount() == 2) {
                // true blackjack even if dealer does not resolve means hand gets disabled
                hand.setFlagIsInTheGame(false);
            }
        }
    }
    // pregame checks over


    if (flagRoundEnd) { // meaning dealer had a blackjack
        gr.dealerHadBlackjack = true; // ensure that we track the fact that the dealer had a blackjack
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
                // update gameKey to ensure correct lookups
                gameKey = makeUniqueKey(players[0].getHands()[i].getValue(), players[0].getHands()[i].getCardsCount(), players[0].getHands()[i].isSoft(), players[0].getHands()[i].isSplittable(), d.getUpCard().getValue(), shoe);
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
                        std::string input = getInput("lateSurrender", gameKey);
                        choice = input[0];
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
                std::cout << player.getName() << "'s balance: " << player.getBalance() << std::endl;
                std::cout << "" << player.getName() << "'s hand " << i + 1 << ": ";
                player.getHands()[i].printHand();


                // determine which actions are allowed, prompt the player with the allowed actions, reject any non-legal action
                //hitting always allowed, standing always allowed, double allowed based on rules, splitting allowed when same pair AND rules
                std::vector<char> allowedChoices = determineAllowedChoices(player.getHands()[i], r, player.getBalance(), player.getHands().size());

                // legal actions determined and safed in allowedChoices vector
                char choice = 'f';
                while (std::find(allowedChoices.begin(), allowedChoices.end(), choice) == allowedChoices.end()) {
                    std::cout << "Available actions: ";
                    for (char action : allowedChoices) {
                        switch (action) {
                        case 'h': std::cout << "Hit (h) "; 
                            break;
                        case 's': std::cout << "Stand (s) "; 
                            break;
                        case 'd': std::cout << "Double (d) "; 
                            break;
                        case 'p': std::cout << "Split (p) "; 
                            break;

                        default: std::cout << action << " "; 
                            break;
                        }
                    }
                    std::cout << std::endl << "Choice: ";

                    std::string input = getInput("choice", gameKey, allowedChoices);
                    choice = input[0];
                    if (std::find(allowedChoices.begin(), allowedChoices.end(), choice) == allowedChoices.end()) {
                        std::cout << "Invalid choice!" << std::endl;
                        std::cout << gameKey;
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
                    std::cout << player.getName() << "'s balance: " << player.getBalance() << std::endl;
                }
                else if (choice == 'p') {
                    // remove the second card of the hand, create a new hand object and add that card, then deal 2 new cards
                    int endOfVector = player.getHands().size();
                    player.addHand(); // we have hand and hand2 to work with

                    player.getHands()[i].setFlagIsSplitHand(true);
                    player.getHands()[endOfVector].setFlagIsSplitHand(true);
                    player.getHands()[endOfVector].addCard(player.getHands()[i].removeCard(1)); // remove it and add it to the other hand, then redeal new cards to those hands
                    player.getHands()[i].addCard(shoe.deal());
                    player.getHands()[endOfVector].addCard(shoe.deal());
                    player.getHands()[endOfVector].setBet(player.getHands()[i].getBet()); // match the bet of the new hand to the old one
                    player.setBalance(player.getBalance() - player.getHands()[i].getBet());
                    j = 0; // reset the iteration count because we basically are at the start of the game playing 2 hands(important for asking if we want to surrender)
                    gr.playerResults[player.getName()].handResults.emplace_back(); // create an additional handResults struct for the hand we just created
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
    // give second card when playing ENHC
    if (not r.peakAmericanStyle) { d.addCard(getStarterCard("dealer2")); }
    std::cout << "Dealer: ";  d.getHand().printHand();
    while (d.getHand().getValue() < 17 or (d.getHand().getValue() == 17 and d.getHand().isSoft() and r.hitSoft17)) { // soft17 = false means h17 rules so hitting
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
            handResult& hres = gr.playerResults[player.getName()].handResults[i];
            hres.isSplitHand = hand.getFlagIsSplitHand(); // add that info to the return
            hres.handBet = hand.getBet(); // also important for EV calc
            // TODO: New EV calculation that takes into account handBet. New variable totalWagered, for split hands add up all bets for all hands in each round. Derive the winnings
            if (player.getHands()[i].getSideBet("insurance")) {
                // if we took insurance
                if (d.getHand().getValue() == 21 and d.getHand().getCardsCount() == 2) {
                    // and dealer has blackjack
                    // we win our insurance bet
                    double payout = hand.getSideBet("insurance") * 3; // this is only the insurance payout, real hand comes later
                    std::cout << "Dealer had blackjack, " << player.getName() << "'s hand " << i + 1 << " insurance payout: " << payout << "\n"; // sidebet * 2
                    player.setBalance(player.getBalance() + payout);

                    hres.insuranceOutcome = InsuranceOutcome::TakenWon;
                    hres.insuranceWinnings += payout - hand.getSideBet("insurance");
                    
                }
                else {
                    // and dealer doesnt have blackjack
                    // we lose our insurance bet
                    double payout = 0; // this is only the insurance payout, real hand comes later
                    std::cout << "Dealer didn't have blackjack, " << player.getName() << "'s hand " << i + 1 << " insurance payout: " << payout << "\n"; // 0
                    player.setBalance(player.getBalance() + payout);
                    hres.insuranceOutcome = InsuranceOutcome::TakenLost;
                    hres.insuranceWinnings += payout-hand.getSideBet("insurance");
                }
            }
            // sidebet resolution over

            if (hand.getFlagSurrendered()) { // we surrendered the hand, return bet/2
                double payout = hand.getBet() / 2;
                std::cout << player.getName() << "'s hand " << i + 1 << " had surrendered, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);

                hres.handOutcome = HandOutcome::surrendered;
                hres.handWinnings += payout - hand.getBet();
            }
            else if (hand.getValue() > 21) {
                // we lose
                double payout = 0;
                std::cout << player.getName() << "'s hand " << i + 1 << " \033[31mloses\033[0m, payout: " << payout << "\n";
                hres.handOutcome = HandOutcome::lostByBust;
                hres.handWinnings += payout - hand.getBet();

            }
            else if (hand.getValue() == 21 and hand.getCardsCount() == 2 and not hand.getFlagIsSplitHand()) { // this is a natural blackjack
                if (hand.getFlagEvenMoney()) {
                    // we have true blackjack with even money, return bet twice because of the 1:1 payout
                    double payout = hand.getBet() * 2;
                    std::cout << player.getName() << "'s hand " << i + 1 << " had even money, payout: " << payout << "\n";
                    player.setBalance(player.getBalance() + payout);

                    hres.handOutcome = HandOutcome::evenMoney;
                    hres.handWinnings += payout - hand.getBet();
                }
                else {
                    // we have true blackjack without even money, compare with dealer value to determine payout(push or blackjack)
                    if (d.getHand().getValue() == 21 && d.getHand().getCardsCount() == 2) {
                        // push blackjack
                        double payout = hand.getBet();
                        std::cout << player.getName() << "'s hand " << i + 1 << " pushes blackjack, payout: " << payout << "\n";
                        player.setBalance(player.getBalance() + payout);
                        hres.handOutcome = HandOutcome::pushByDealerBlackjack;
                        hres.handWinnings += payout - hand.getBet();
                    }
                    else {
                        // BLACKJACK
                        double payout = hand.getBet() + hand.getBet() * r.blackjackPayoutRatio;
                        std::cout << player.getName() << "'s hand " << i + 1 << " wins blackjack, payout: " << payout << "\n";
                        player.setBalance(player.getBalance() + payout);
                        hres.handOutcome = HandOutcome::blackjackWin;
                        hres.handWinnings += payout - hand.getBet();
                    }
                }
            }
            else if (hand.getValue() == 21 and hand.getCardsCount() > 2 and d.getHand().getValue() == 21 && d.getHand().getCardsCount() == 2 and not r.peakAmericanStyle) {
                // exception with european peek, if we get to 21 (possible because dealer doesnt peek) with more than 2 cards and the dealer does have a natural blackjack, we lose
                double payout = 0;
                std::cout << player.getName() << "'s hand " << i + 1 << " \033[31mloses\033[0m, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
                hres.handOutcome = HandOutcome::lostBy21AgainstDealerBlackjack;
                hres.handWinnings += payout - hand.getBet();
            }
            else if (hand.getValue() <= 21 and d.getHand().getValue() > 21) {
                // win
                double payout = hand.getBet() * 2;
                std::cout << player.getName() << "'s hand " << i + 1 << " 	\033[32mwins\033[0m, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
                hres.handOutcome = HandOutcome::wonByDealerBust;
                hres.handWinnings += payout - hand.getBet();
            }
            else if (hand.getValue() > d.getHand().getValue()) {
                // win
                double payout = hand.getBet() * 2;
                std::cout << player.getName() << "'s hand " << i + 1 << " 	\033[32mwins\033[0m, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
                hres.handOutcome = HandOutcome::wonByHigherValue;
                hres.handWinnings += payout - hand.getBet();
            }
            else if (hand.getValue() == d.getHand().getValue()) {
                // push
                double payout = hand.getBet();
                std::cout << player.getName() << "'s hand " << i + 1 << " \033[35mpushes\033[0m, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
                hres.handOutcome = HandOutcome::pushBySameValue;
                hres.handWinnings += payout - hand.getBet();
            }
            else if (hand.getValue() < d.getHand().getValue()) {
                // lose
                double payout = 0;
                std::cout << player.getName() << "'s hand " << i + 1 << " \033[31mloses\033[0m, payout: " << payout << "\n";
                player.setBalance(player.getBalance() + payout);
                hres.handOutcome = HandOutcome::lostByLowerValue;
                hres.handWinnings += payout - hand.getBet();
            }
        }
        std::cout << player.getName() << "'s new balance: " << player.getBalance() << "\n";


    }
    // after all hands have been evaluated, clear all hands and the table. Reset all bets to 0 by default. 
    // seperate loop because in real blackjack, hands are all taken away at once
    // remove any split hands and set all flags to false
    d.clearHand();
    for (Player& player : players) {
        player.removeAllHands();
    }
    return gr;
}
