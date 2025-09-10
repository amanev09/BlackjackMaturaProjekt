#include <iostream>
#include <vector>

#include "Game.h"
#include "strategySimulation.h"
#include "Online_stats.h"

void strategySimulation(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    bj::OnlineStats stats;

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
    double counts = 0;
    for (int i = 0; i < iterations; ++i) {
        //shoe.reset();
        // optionally modify deck here
        //shoe.removeCard('2', 'S');
        counts+= shoe.setHiLoTrueCount(6);
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, nullptr, strategy); // play one game of blackjack using a premade strategy(for example basic strategy)
        
        auto& hres = gr.playerResults["Nevio"];
        double initialBet = std::stod(inputMap["numHands"]) * std::stod(inputMap["bet"]);
        double netProfit = 0;
        for (handResult& h : hres.handResults) {netProfit += h.handWinnings;}
        double x = netProfit / initialBet;
        stats.add(x); // averages the EV results of my simulation
    }

    double EV = stats.mean;
    double variance = stats.variance();
    double sd = stats.sd();
    double sem = stats.sem();

    std::cout << "Total strategy EV: " << 100.0* EV <<"% with a standard error of the mean of "<<sem*100<<"%, average hiLo count was: "<< counts / (static_cast<double>(iterations)) << "\n";
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Standard deviation: " << sd << std::endl;
}