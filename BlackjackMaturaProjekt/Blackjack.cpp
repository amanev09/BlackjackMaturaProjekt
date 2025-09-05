#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"
#include "basicStrategy.h"
#include "strategySimulation.h"
#include "IOStrategy.h"
#include "countingSim.h"
int main()
{
    std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();

    std::unordered_map<std::string, Card> startingCards = { 
    {"player1", Card('A', 'S')},
    {"player2", Card('A', 'S')},
    {"dealer1", Card('A', 'S')},
        {"dealer2", Card('K', 'S')}
    };

    for (int i = 0; i < 1000; ++i) { blackJack(r, players, d, shoe, nullptr, nullptr, nullptr); }
    bool calc = false;

    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> strategy;
    if (calc) {
        strategy = basicStrategy();
        saveStrategy(strategy, "basicStrategyForSimulation");
    }
    else {
        loadStrategy(strategy, "basicStrategyForSimulation");
    }
    HiLoSim(&strategy);
    strategySimulation(&strategy);
    return 0;
}
