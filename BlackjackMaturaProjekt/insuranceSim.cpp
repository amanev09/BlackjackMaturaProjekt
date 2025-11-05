#include <iostream>
#include <vector>

#include "insuranceSim.h"
#include "Game.h"
#include "strategySimulation.h"
#include "Online_stats.h"
#include "countingStrategyEnum.h"

void insuranceSimulation() {

    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    bj::OnlineStats stats;


    std::unordered_map<std::string, std::string> inputMap = { // only insurance matters here
    {"numHands", "1"},
    {"bet", "1"},
    {"earlySurrender", "n"},
    {"evenMoney", "y"},
    {"insurance", "y"},
    {"lateSurrender", "n"},
    {"choice", "s"}
    };

    std::unordered_map<std::string, Card> startingCards = { 
    {"dealer1", Card('A', 'S')},
    {"player1", Card('5', 'S') },
    {"player2", Card('6', 'S') }
    };

    int iterations = 1000000;
    double counts = 0;
    double pTens = 0;
    double insuranceBet = std::stod(inputMap["bet"]) / 2;
    for (int i = 0; i < iterations; ++i) {
        counts+= shoe.setHiLoTrueCount(3.4); // 3.4 is breakeven 
        pTens += shoe.getPTen();
        Card holeCard = shoe.getCardsVector().back();

        gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, nullptr); // strategy doesnt matter, we always stand our hand and only analyse insurance outcome
        auto& hres = gr.playerResults["Nevio"];
        double netInsuranceProfit = hres.handResults[0].insuranceWinnings; // we can take 0 because we have only one hand guaranteed. Unsafe but too lazy to do otherwise
        double x = netInsuranceProfit / insuranceBet;
        //std::cout << "\nnetInsuranceProfit: " << netInsuranceProfit << ", insuranceBet: " << insuranceBet << ", x(EV): " << x << std::endl;
        stats.add(x);
    }
    double EV = stats.mean;
    double variance = stats.variance();
    double sd = stats.sd();
    double sem = stats.sem();
    std::cout << "Combinatorial EV of insurance: " << (3 * (pTens / iterations) - 1)*100<<"%" << std::endl;
    std::cout << "Total insurance EV: " << 100.0 * EV << "% with a standard error of the mean of " << sem * 100<<"%" << " average hiLo count was: " << counts / (static_cast<double>(iterations)) << "\n";
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Standard deviation: " << sd << std::endl;
}