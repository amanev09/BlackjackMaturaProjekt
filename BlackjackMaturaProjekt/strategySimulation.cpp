#include <iostream>
#include <vector>

#include "Game.h"
#include "strategySimulation.h"

void strategySimulation(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();
    std::unordered_map<std::string, std::string> inputMap = { // for info like numHands or bet, not for actual choice making. Confirm by setting irrelevant entries to gibberish
    {"numHands", "1"},
    {"bet", "1"},
    {"earlySurrender", "n"},
    {"evenMoney", "n"},
    {"insurance", "f"},
    {"lateSurrender", "n"},
    {"choice", "f"}
    };
    int iterations = 1000000;
    double preGameBalance = players[0].getBalance();
    for (int i = 0; i < iterations; ++i) {
        shoe.reset();
        shoe.shuffle();
        // optionally modify deck here
        shoe.removeCard('J', 'S');
        shoe.removeCard('K', 'H');
        shoe.removeCard('Q', 'D');
        shoe.removeCard('K', 'C');
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, nullptr, strategy); // play one game of blackjack using a premade strategy(for example basic strategy)
    }
    double postGameBalance = players[0].getBalance();
    std::cout << "Total strategy EV: " << (postGameBalance - preGameBalance) / iterations<<"\n";
    
}