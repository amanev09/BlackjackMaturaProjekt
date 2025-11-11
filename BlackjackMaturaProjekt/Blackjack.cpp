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
#include "insuranceSim.h"
#include "deviationSimulation.h"
int main()
{
    //std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);

    std::unordered_map<std::string, Card> startingCards = { 
    {"player1", Card('8', 'S')},
    {"player2", Card('8', 'S')},
    {"dealer1", Card('A', 'S')},
    //{"dealer2", Card('K', 'S')}
    };
    /* splitting EV too high for some reason(maybe with EV calc totalProfit / initialBet) 
    asking for insurance before printing the hand(potentially also with early surrender) only visual tho. DONE
    Adding feature in shoe to configure the shoe with random composition for a given true count to simulate changes between HiLo true counts. DONE
    Taking insurance when true count is good enough in countingSim(ev impact?) including overall insurance EV. Also Insurance EV bug DONE
    Analysing basic strategy deviations based on count (HiLo), add them to counting sim(EV impact?), this means create multiple basic strategy charts and loop over differences
    Bet sizing, bankroll management and risk of ruin.
    Adding other counting methods and looking at EV impact(inlcuding deviations for those systems), potentially too time consuming and ambitious for presentation+paper
    */

    //for (int i = 0; i < 1000; ++i) {
        //shoe.setHiLoTrueCount(0); //std::cout << shoe.getPTen() << "\n"; blackJack(r, players, d, shoe, nullptr, &startingCards, nullptr); } // inputChoice, startingcards, cache 
    

        bool calc = false;
        std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> strategy;
        std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> devStrat;
        if (calc) {
            strategy = basicStrategy();
            saveStrategy(strategy, "basicStrategyForSimulationCount1");
        }
        else {
            loadStrategy(strategy, "basicStrategyForSimulation");
        }
        //devStrat = deviationStrategy(&strategy);
        strategySimulation(&strategy);
        HiLoSim(&strategy);
        //insuranceSimulation();
    
    return 0;
}
