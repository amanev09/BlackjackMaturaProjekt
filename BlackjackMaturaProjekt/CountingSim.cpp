#include "countingSim.h"

void HiLoSim(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {
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
    int iterations = 10000000;
    double preGameBalance = players[0].getBalance();
    for (int i = 0; i < iterations; ++i) {
        //shoe.reset();
        // optionally modify deck here
        //shoe.removeCard('J', 'S');
        std::unordered_map<int, std::string> betSpread = {
            {0, "1"},
            {1, "2"},
            {2, "3"},
            {3, "5"},
            {4, "10"},
        };
        int trueCount = shoe.getHiLoTrueCount();
        if (betSpread.count(trueCount)) {
            inputMap["bet"] = betSpread.at(trueCount); // otherwise its default in inputMap
        }
        gameResult gr = blackJack(r, players, d, shoe, &inputMap, nullptr, strategy); // play one game of blackjack using a premade strategy(for example basic strategy)
    }
    double postGameBalance = players[0].getBalance();
    std::cout << "Total HiLo + strategy EV: " << 100 * (postGameBalance - preGameBalance) / iterations << "%" << "\n";

}