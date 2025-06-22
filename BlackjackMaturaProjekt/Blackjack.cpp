#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

#include "Game.h"
#include "makeUniqueKey.h"

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
    std::unordered_map<std::string, Card> startingCards = {
        {"player1", Card('T', 'S')},
        {"player2", Card('J', 'H')},
        {"dealer1", Card('A', 'S')}
        //{ "dealer2", Card('7', 'S') }
    };

    double preBank = players[0].getBalance();
    int iterations = 1;
    double wonInsurance = 0;
    double lostInsurance = 0;
    int noInsurance = 0;
    for (int i = 0; i < iterations; ++i) {
        
        shoe.reset();
        shoe.removeCard('T', 'S');
        shoe.removeCard('T', 'H');
        shoe.removeCard('A', 'S'); // modify the shoe composition the way we 
        shoe.shuffle();

        double preGameBalance = players[0].getBalance();
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards); // play one game of blackjack
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
    std::cout << "EV: " << (postBank-preBank)/iterations;
    std::cout << "\nInsurance EV: " << (wonInsurance*2-lostInsurance)/iterations;
    return 0;
}
