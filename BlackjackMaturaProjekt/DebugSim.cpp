#include <iostream>
#include <vector>
#include <fstream>

#include "Game.h"
#include "strategySimulation.h"
#include "Online_stats.h"
#include "countingStrategyEnum.h"

void debugSim(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    bj::OnlineStats stats;


    std::unordered_map<std::string, std::string> inputMap = { // for info like numHands or bet, not for actual choice making. Confirm by setting irrelevant entries to gibberish
    {"numHands", "1"},
    {"bet", "1"},
    {"earlySurrender", "n"},
    {"evenMoney", "n"},
    {"insurance", "f"},
    {"lateSurrender", "n"},
    {"choice", "f"}
    };
    
    std::unordered_map<std::string, Card> startingCards = {
    {"player1", Card('2', 'S')},
    //{"player2", Card('2', 'S')},
    {"dealer1", Card('2', 'S')},
    //{"dealer2", Card('K', 'S')}
    };

    int iterations = 1000000;
    double counts = 0;
    double globalNetProfit = 0;
    int moduluFactor = 20;
    std::vector<double> bankrollHistory;
    for (int i = 0; i < iterations; ++i) {
        //shoe.setHiLoTrueCount(0);
		//shoe.removeCard(startingCards.at("player1").getRank(), 'S'); // remove the starting cards so shoe composition is correct
        //shoe.removeCard(startingCards.at("player1").getRank(), 'S');
        //shoe.removeCard(startingCards.at("dealer1").getRank(), 'S');
        //shoe.shuffle(); // shuffle for homogenous shoe composition
		shoe.setHiLoTrueCount(0);
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, strategy); // play one game of blackjack using a premade strategy(for example basic strategy)
        // played first game, play second game now without reset
		gameResult gr2 = blackJack(r, players, d, shoe, &inputMap, &startingCards, strategy);

        auto& hres = gr.playerResults["Nevio"];
        auto& hres2 = gr2.playerResults["Nevio"];

        double initialBet = std::stod(inputMap["numHands"]) * std::stod(inputMap["bet"]);
        double netProfit = 0;
        for (handResult& h : hres.handResults) { netProfit += h.handWinnings; }
		for (handResult& h : hres2.handResults) { netProfit += h.handWinnings; }
        double x = netProfit / initialBet;
        stats.add(x); // averages the EV results of my simulation
        globalNetProfit += netProfit;
        if (i % moduluFactor == 0) {
            bankrollHistory.push_back(globalNetProfit);
        }
    }

    double EV = stats.mean;
    double variance = stats.variance();
    double sd = stats.sd();
    double sem = stats.sem();

    std::cout << "Total strategy EV: " << 100.0 * EV << "% with a standard error of the mean of " << sem * 100 << "%, average hiLo count was: " << counts / (static_cast<double>(iterations)) << "\n";
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Standard deviation: " << sd << std::endl;
    /*
    std::ofstream bankrollFile("bankrollHistory_strategySimulation.csv");
    bankrollFile << "Round,Bankroll\n";

    for (size_t i = 0; i < bankrollHistory.size(); ++i) {
        bankrollFile << i * moduluFactor << "," << bankrollHistory[i] << "\n";
    }

    bankrollFile.close();
    */
}