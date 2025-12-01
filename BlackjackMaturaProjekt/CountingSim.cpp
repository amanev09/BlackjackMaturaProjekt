#include <fstream>
#include "countingSim.h"
#include "Online_stats.h"
#include "countingStrategyEnum.h"

void HiLoSim(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    bj::WeightedOnlineStats weightedStats;
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
    

    std::unordered_map<int, std::string> betSpread = {
    {0, "1"},
    {1, "1"},
    {2, "2"},
    {3, "4"},
    {4, "8"},
    {5, "16"},
    {6, "32"},
    {7, "64"},
    {8, "100"},
    {9, "100"},
    {10, "100"},
    {11, "100"},
    {12, "100"},
    {13, "100"},
    {14, "100"},
    {15, "100"},
    {16, "100"},
    };
    int iterations = 30000000;
    double globalNetProfit = 0;
    int moduluFactor = 1;
    std::vector<double> bankrollHistory;
    for (int i = 0; i < iterations; ++i) {
        //shoe.reset();
        // optionally modify deck here
        //shoe.removeCard('J', 'S');
        int trueCount = shoe.getHiLoTrueCount();
        if (betSpread.count(trueCount)) {
            inputMap["bet"] = betSpread.at(trueCount); // otherwise its default in inputMap
        }
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, nullptr, strategy); // play one game of blackjack using a premade strategy(for example basic strategy)

        auto& hres = gr.playerResults["Nevio"];
        double initialBet = 0;
        if (betSpread.count(trueCount)) {
            initialBet = std::stod(betSpread.at(trueCount));
        }
        else {
            initialBet += std::stod(inputMap["bet"]);
        }
         
        double netProfit = 0;
        for (handResult& h : hres.handResults) { netProfit += (h.handWinnings+h.insuranceWinnings);} // EV calculated per initial wager, so no need to incclude insurance wager in calc.
        double x = netProfit / initialBet;
        weightedStats.add(x, initialBet);
        stats.add(netProfit);
		globalNetProfit += netProfit;
        if (i % moduluFactor == 0) {
            bankrollHistory.push_back(globalNetProfit);
        }
    }
    double EV = weightedStats.weightedMean;
    double variance = weightedStats.variance();
    double sd= weightedStats.sd();
    double sem = weightedStats.sem();

    double BUEV = stats.mean;
	double BUvariance = stats.variance();
	double BUsd = stats.sd();
	double BUsem = stats.sem();

    std::cout << "Total HiLo + strategy EV: "<<EV * 100 << "%" <<" with a standard error of the mean of: "<<sem*100<<"%" << "\n";
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Standard  deviation: " << sd << std::endl;

    std::cout << "Total HiLo + strategy EV (betting units): " << BUEV << " with a standard error of the mean of: " << BUsem << "\n";
    std::cout << "Variance: " << BUvariance << std::endl;
    std::cout << "Standard  deviation: " << BUsd << std::endl;

    std::ofstream bankrollFile("bankrollHistory_countingSim.csv");
    bankrollFile << "Round,Bankroll\n";

    for (size_t i = 0; i < bankrollHistory.size(); ++i) {
        bankrollFile << i * moduluFactor << "," << bankrollHistory[i] << "\n";
    }

    bankrollFile.close();
}