#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"

int main()
{
    std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();
    std::unordered_map<std::string, std::string> inputMap = { 
        {"numHands", "1"},
        {"bet", "1"},
        {"earlySurrender", "n"},
        {"evenMoney", "n"},
        {"insurance", "n"},
        {"lateSurrender", "n"},
        {"firstChoice", "s"}
    };
    std::unordered_map<std::string, Card> startingCards = { // empty because loop overrides it
    };

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> cache; // gameState, corresponding inputMap
    /*
    The cache contains other input maps for a given key. The idea is that the program saves the best inputMap combination for a given situation(key) inside the cache.
    When the program later needs to lookup the best move for a given situation, it simply generates the key for the situation and uses it to lookup the cached inputMap and then uses that.
    */

    double preBank = players[0].getBalance();
    int iterations = 1;
    double wonInsurance = 0;
    double lostInsurance = 0;
    int noInsurance = 0;
    // get choices we can simulate for a given hand

    std::vector<Hand> handsToSimulate; // contains all hands that are to be simulated. Order: First element is the first hand to simulate.

    // first, manually add arbitrary hard 21 with more than 2 cards

    handsToSimulate.emplace_back();
    Hand& h21 = handsToSimulate.back(); // grab reference to hand we just added
    h21.addCard(Card('7', 'S'));
    h21.addCard(Card('7', 'S'));
    h21.addCard(Card('7', 'S')); // triple 7 lucky number lol

    for (int i = 20; i >= 5; i--) { // exclude 4 because the only way to build 4 with two cards is 2,2 and that hand is splittable. Postpone simulation(not an issue)
        // start with i = 20 which is the value we need to achieve for the current hand. Figure out the card combination, build the hand and add it into the vector
        for (int j = 10; j >= 2; j--) {
            // to figure out first card: loop all cards and figure out the highest value card that ensures that the remainder that the second card needs to fill is equal or greater than 2
            if (i - j >= 2) { // if remainder is greater or equal 2, j is our first card value
                char c1;
                char c2;
                if (j == 10) {
                    c1 = 'T'; // set it manually to ten because '10' is not a valid argument for the card constructor
                }
                else {
                    c1 = '0' + j; // '0' + integer = correct char if 1-9.
                }
                if (i - j == 10) {
                    c2 = 'K'; // arbitrary other 10 value card
                }
                else {
                    c2 = '0' + (i - j);
                }
                handsToSimulate.emplace_back();
                Hand& h = handsToSimulate.back(); // grab reference to hand we just added
                h.addCard(Card(c1, 'S'));
                h.addCard(Card(c2, 'S'));
                break; // break out of inner loop because we have found the card combination
            }
        }
    }
    // sucessfully added all hard hands, correct combination and order.
    // continue with all soft hands.
    //begin by manually adding soft 21 with more than 2 cards

    handsToSimulate.emplace_back(); // simply emplace back and all the soft hands get added
    Hand& s21 = handsToSimulate.back(); // grab reference to hand we just added
    s21.addCard(Card('A', 'S'));
    s21.addCard(Card('5', 'S'));
    s21.addCard(Card('5', 'S'));

    for (int i = 20; i >= 13; i--) { // smallest soft total is 13: 11,2
        char c1 = 'A'; // first card is always an ace
        // second card is just i-j(always lower than 10)
        char c2 = '0' + (i - 11); // remainder to fill

        handsToSimulate.emplace_back(); // simply emplace back and all the soft hands get added
        Hand& h = handsToSimulate.back(); // grab reference to hand we just added
        h.addCard(Card(c1, 'S'));
        h.addCard(Card(c2, 'S'));
    }
    // finish by adding all splittable hands, note that because K,K =  T,T; only one out of 4 will be simulated
    for (int i = 2; i <= 9; i++) {
        char c = '0' + i;
        handsToSimulate.emplace_back(); // simply emplace back and all the soft hands get added
        Hand& h = handsToSimulate.back(); // grab reference to hand we just added
        h.addCard(Card(c, 'S'));
        h.addCard(Card(c, 'S'));
    }
    // add T,T and A,A because they are letters
    handsToSimulate.emplace_back(); // simply emplace back and all the soft hands get added
    Hand& tenHand = handsToSimulate.back(); // grab reference to hand we just added
    tenHand.addCard(Card('T', 'S'));
    tenHand.addCard(Card('T', 'S'));
    handsToSimulate.emplace_back(); // simply emplace back and all the soft hands get added
    Hand& aceHand = handsToSimulate.back(); // grab reference to hand we just added
    aceHand.addCard(Card('A', 'S'));
    aceHand.addCard(Card('A', 'S'));
    // vector handsToSimulate now fully populated with all hands we want to simulate


    for (Hand& h : handsToSimulate) {
        std::vector<char> allowedChoices = determineAllowedChoices(h, r, players[0].getBalance(), players[0].getHands().size()); // need to figure out what choices we can simulate for the given hand
        // set the startingCards unordered_map
        startingCards.clear(); // first, clear out the starting cards, then add all of the correct starting cards

        for (int i = 0; i < h.getCardsCount(); ++i) {
            // loop through all cards of the hand
            std::string  key = "player" + std::to_string(i + 1);
            startingCards.emplace(key, h.getCard(i));

        }
        // assign correct starting card to the dealer too
        for (int i = 11; i >= 2; --i) {
            char c;
            if (i >= 2 and i <= 9) {
                c = '0' + i;
            }
            if (i == 10) {
                c = 'T';
            }
            if (i == 11) {
                c = 'A';
            }
            
            startingCards.insert_or_assign("dealer1", Card(c, 'S')); // assign when it doesnt exist, insert when it does
            
            // correct starting cards now assigned

            for (char choice : allowedChoices) {
                // ensure correct inputMap, currently only change 'firstChoice' entry
                inputMap["firstChoice"] = choice;
                for (int i = 0; i < iterations; ++i) {
                    // main simulation loop
                    shoe.reset();
                    //purposefully dont remove added cards for now
                    shoe.shuffle();

                    double preGameBalance = players[0].getBalance();
                    gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, &cache); // play one game of blackjack
                    if (gr.playerResults["Nevio"].handResults[0].insuranceOutcome == InsuranceOutcome::TakenWon) {

                        wonInsurance++;
                    }
                    else if (gr.playerResults["Nevio"].handResults[0].insuranceOutcome == InsuranceOutcome::TakenLost) {
                        lostInsurance++;
                    }
                    else {
                        noInsurance++;
                    }
                    if (gr.dealerHadBlackjack == true) {
                        // discard the round
                        i--;
                        players[0].setBalance(preGameBalance); // reset balance
                    }

                }
                double postBank = players[0].getBalance();
                std::cout << "EV: " << (postBank - preBank) / iterations;
                std::cout << "\nInsurance EV: " << (wonInsurance * 2 - lostInsurance) / iterations;
            }
        }
    }


    return 0;
}
