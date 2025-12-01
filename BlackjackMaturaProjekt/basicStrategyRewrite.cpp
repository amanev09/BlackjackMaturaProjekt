// calculates basic strategy
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <map>
#include <algorithm>
#include "fstream"

#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"
#include "basicStrategy.h"
#include "countingStrategyEnum.h"
#include "basicStrategyRewrite.h"
#include "Online_stats.h"

std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> basicStrategyRewrite() {
    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);

    std::unordered_map<std::string, std::string> inputMap = {
    {"numHands", "1"}, // Important: correct EV calculation only works when playing with one hand for now
    {"bet", "1"},
    {"earlySurrender", "n"},
    {"evenMoney", "n"},
    {"insurance", "n"},
    {"lateSurrender", "n"},
    {"choice", "f"}
    };

    std::unordered_map<std::string, Card> startingCards = { // empty because loop overrides it
    };
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache; // gameState, corresponding inputMap, used by simulation
    std::vector<Hand> playerHandsToSimulate = { // done manually because the algorithm takes up more space than this
    Hand({Card('7', 'S'), Card('7', 'S'), Card('7', 'S')}), 
    Hand({Card('7', 'S'), Card('7', 'S'), Card('6', 'S')}), 
    Hand({Card('9', 'S'), Card('J', 'S')}), 
    Hand({Card('8', 'S'), Card('J', 'S')}), 
    Hand({Card('8', 'S'), Card('9', 'S')}),
    Hand({Card('7', 'S'), Card('9', 'S')}), 
    Hand({Card('6', 'S'), Card('9', 'S')}), 
    Hand({Card('5', 'S'), Card('9', 'S')}), 
    Hand({Card('4', 'S'), Card('9', 'S')}), 
    Hand({Card('3', 'S'), Card('9', 'S')}), 
    Hand({Card('2', 'S'), Card('9', 'S')}), // hard 11
    Hand({Card('4', 'S'), Card('6', 'S'), Card('A', 'S')}), // soft values need atleast hard 12
    Hand({Card('A', 'S'), Card('9', 'S')}), 
    Hand({Card('A', 'S'), Card('8', 'S')}), 
	Hand({Card('A', 'S'), Card('7', 'S')}), 
	Hand({Card('A', 'S'), Card('6', 'S')}), 
	Hand({Card('A', 'S'), Card('5', 'S')}), 
	Hand({Card('A', 'S'), Card('4', 'S')}),
	Hand({Card('A', 'S'), Card('3', 'S')}), 
	Hand({Card('A', 'S'), Card('2', 'S')}), 
    Hand({Card('2', 'S'), Card('8', 'S')}), // hard values below 10 need all soft values except A,A
    Hand({Card('2', 'S'), Card('7', 'S')}), 
    Hand({Card('2', 'S'), Card('6', 'S')}), 
    Hand({Card('2', 'S'), Card('5', 'S')}), 
    Hand({Card('2', 'S'), Card('4', 'S')}), 
    Hand({Card('2', 'S'), Card('3', 'S')}),  
    Hand({Card('A', 'S'), Card('A', 'S')}), // split vals now
    Hand({Card('A', 'S'), Card('A', 'S')}), // sim twice
    Hand({Card('K', 'S'), Card('K', 'S')}),
    Hand({Card('9', 'S'), Card('9', 'S')}),
    Hand({Card('8', 'S'), Card('8', 'S')}),
    Hand({Card('7', 'S'), Card('7', 'S')}),
    Hand({Card('6', 'S'), Card('6', 'S')}),
    Hand({Card('5', 'S'), Card('5', 'S')}),
    Hand({Card('4', 'S'), Card('4', 'S')}),
    Hand({Card('3', 'S'), Card('3', 'S')}),
    Hand({Card('2', 'S'), Card('2', 'S')}),
    Hand({Card('2', 'S'), Card('2', 'S')}) // sim twice
    };

    std::vector<Hand> dealerHandsToSimulate = { // done manually because the algorithm takes up more space than this
    Hand({Card('A', 'S')}),
    Hand({Card('T', 'S')}),
    Hand({Card('9', 'S')}),
    Hand({Card('8', 'S')}),
    Hand({Card('7', 'S')}),
    Hand({Card('6', 'S')}),
    Hand({Card('5', 'S')}),
    Hand({Card('4', 'S')}),
    Hand({Card('3', 'S')}),
    Hand({Card('2', 'S')}),
    };

    int iterations = 10000;
    int doubleAAIterations = 0;
    int doubleTWOTWOIterations = 0;
    int originalSplitSetting = r.maxSplitHands;
    for (Hand& ph : playerHandsToSimulate) {
        std::vector<char> allowedChoices = determineAllowedChoices(ph, r, players[0].getBalance(), players[0].getHands().size());
		startingCards.clear();
        for (int i = 0; i < ph.getCardsCount(); ++i) { // fill the startingCards vector
            std::string key = "player" + std::to_string(i + 1);
            startingCards.emplace(key, ph.getCard(i));
        }
        for (Hand& dh : dealerHandsToSimulate) {
            startingCards.insert_or_assign("dealer1", dh.getCard(0)); // assign when it doesnt exist, insert when it does
			std::string uniqueKey = makeUniqueKey(ph.getValue(), 2, ph.isSoft(), ph.isSplittable(players.back().getHands().size()), startingCards.at("dealer1").getValue(), shoe); // current game state key
            std::string uniqueKeyMultiCard = makeUniqueKey(ph.getValue(), 3, ph.isSoft(), ph.isSplittable(players.back().getHands().size()), startingCards.at("dealer1").getValue(), shoe);

            if (doubleAAIterations >= 10) {
                r.maxSplitHands = originalSplitSetting; // reset to original setting 
            }
            if (uniqueKey.find("12;2;1;1;") != std::string::npos and doubleAAIterations < 10) {
                doubleAAIterations++;
				uniqueKey = "12;2;1;0;" + std::to_string(startingCards.at("dealer1").getValue()) + ";"; 
                r.maxSplitHands = 1; // disable to simulate necessary situation 
				// after 10 times, meaning when all dealer hands are simulated, we can reset the split setting
                if (allowedChoices.back() == 'p') {
                    allowedChoices.pop_back(); // remove split option
                }
            }
            if (doubleTWOTWOIterations >= 10) {
                r.maxSplitHands = originalSplitSetting; // reset to original setting 
            }
            if (uniqueKey.find("4;2;0;1;") != std::string::npos and uniqueKey.find("14;2;0;1;") == std::string::npos and doubleTWOTWOIterations < 10) {
                doubleTWOTWOIterations++;
                uniqueKey = "4;2;0;0;" + std::to_string(startingCards.at("dealer1").getValue()) + ";";
                r.maxSplitHands = 1; // disable to simulate necessary situation 
                // after 10 times, meaning when all dealer hands are simulated, we can reset the split setting
                if(allowedChoices.back() == 'p') {
                    allowedChoices.pop_back(); // remove split option
				}
            }

            // if hand is splittable, uniqueKeyMultiCard entry cannot exist in a real game and is a dead entry. 
            std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> miniCache; // holds the inputMaps with corresponding EV values for the current game state
            
            for (char choice : allowedChoices) {
                inputMap["choice"] = choice;
                double totalHandWager = 0;
                double totalHandWinnings = 0;
                for (int i = 0; i < iterations; ++i) {
                    shoe.setHiLoTrueCount(0);
					shoe.removeCard(startingCards.at("player1").getRank(), 'S');
                    shoe.removeCard(startingCards.at("player2").getRank(), 'S');
					shoe.removeCard(startingCards.at("dealer1").getRank(), 'S');
                    shoe.shuffle(); 
                    gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, &cache); // play one game of blackjack
                    auto& hres = gr.playerResults["Nevio"];

                    if (not(gr.dealerHadBlackjack and r.peakAmericanStyle)) {
                        totalHandWager += 1; // only count initial bets as wagers, exclude splits and doubles
                        for (handResult& h : hres.handResults) {
                            totalHandWinnings += h.handWinnings; // count both double and split winnings
                        }
                    }
                    if (gr.dealerHadBlackjack and r.peakAmericanStyle) { i--; } // redo the simulation round
                }
                double baseEV = totalHandWinnings / totalHandWager;
                std::cout << uniqueKey << "base ev for " << choice << ": " << baseEV << "\n";
                miniCache.emplace_back(baseEV, inputMap);
            }
            std::sort(miniCache.begin(), miniCache.end(),
                [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

            if (!miniCache.empty() && r.lateSurrenderAllowed) {
                const auto& bestPair = miniCache.front();
                if (bestPair.first < -0.5) { // lateSurrender has better EV than best choice
                    auto surrenderMap = bestPair.second;
                    surrenderMap["lateSurrender"] = "y";
                    surrenderMap["choice"] = "r"; // set to r so we know to remove for 3+ hands
                    miniCache.emplace(miniCache.begin(), -0.5, std::move(surrenderMap)); // add it as the first element
                    // if the otherwise best choice has an ev of less than -0.5, the surrenderMap is added.
                }
            }
			cache[uniqueKey] = miniCache;

            // remove options for 3+ cards (double, split, surrender)
            miniCache.erase(
                std::remove_if(
                    miniCache.begin(),
                    miniCache.end(),
                    [](const auto& entry) {
                        const auto& choice = entry.second.at("choice");
                        return choice == "d" || choice == "p" || choice == "r";
                    }
                ),
                miniCache.end()
            );
            cache[uniqueKeyMultiCard] = miniCache;
        }
    }

    std::ofstream basicStrategyFile("basicStrategy.csv");
    basicStrategyFile << "HardTotals,2,3,4,5,6,7,8,9,T,A\n";
    for (int i = 20; i >= 5; --i) {
        basicStrategyFile << i;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, false, j, shoe);
            if (cache.count(k)) {
                std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
                basicStrategyFile << "," << c;
            }
        }
        basicStrategyFile << "\n";
    }
    basicStrategyFile << "SoftTotals,2,3,4,5,6,7,8,9,T,A\n";
    for (int i = 9; i >= 2; --i) {
        basicStrategyFile << "A" << i;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(11 + i, 2, true, false, j, shoe);
            if (cache.count(k)) {
                std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
                basicStrategyFile << "," << c;
            }
        }
        basicStrategyFile << "\n";
    }
    basicStrategyFile << "PairTotals,2,3,4,5,6,7,8,9,T,A\n";

    basicStrategyFile << "A;A,";
    for (int j = 2; j <= 11; j++) {
        std::string k = makeUniqueKey(12, 2, true, true, j, shoe);
        if (cache.count(k)) {
            std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
            basicStrategyFile << c;
        }
        basicStrategyFile << ",";
    }
    basicStrategyFile << "\n";
    for (int i = 20; i >= 4; i -= 2) {
        basicStrategyFile << i/2<<";"<<i/2;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, true, j, shoe);
            if (cache.count(k)) {
                std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
                basicStrategyFile << "," << c;
            }
        }
        basicStrategyFile << "\n";
    }
    

    return cache;
}