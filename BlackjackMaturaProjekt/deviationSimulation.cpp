#include "deviationSimulation.h"
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "Game.h"
#include "strategySimulation.h"
#include "Online_stats.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"
#include "basicStrategy.h"

/*
	Two main options to calculate deviations
	1) Calculate each deviation for each node individually against fixed basic strategy backdrop, but this means that cross deviation interactions are not captured and might distort
	2) Calculate each deviation for each note in unison against basic strategy + previously calculated deviations, but this means that when only using most impactful deviations, distortions
	occur.
	The only way to do it perfectly is to do step 2, then combinatorially combine different deviations, recalculate. Computationally so intensive its not possible.
	2) is best choice therefore
*/

/*
	deviations act as another layer on top of strategy. If deviations exist, they have priority over normal strategy.
	Format is [key]{indexpoint 1 = choice1, indexpoint2 = choice2}. 
	Whatever bracket true count falls in will be choice during simulation. 
*/

/*
    
*/


std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> deviationStrategy(
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy) {

    //std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();
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
        {"player1", Card('7', 'S')},
        {"player2", Card('K', 'S')},
        {"dealer1", Card('A', 'S')},
    };

    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> deviationsCache;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> deviationsVector; // gamestate, all corresponding deviations stored as inputMap. Used by sim
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> otherMiniCache; // Input maps with corresponding EV value, used by simulation
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache;
    
    std::vector<Hand> handsToSimulate = { // done manually because the algorithm takes up more space than this
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
        Hand({Card('2', 'S'), Card('9', 'S')}),
        Hand({Card('2', 'S'), Card('8', 'S')}),
        Hand({Card('2', 'S'), Card('7', 'S')}),
        Hand({Card('2', 'S'), Card('6', 'S')}),
        Hand({Card('2', 'S'), Card('5', 'S')}),
        Hand({Card('2', 'S'), Card('4', 'S')}),
        Hand({Card('2', 'S'), Card('3', 'S')}), // soft vals now
        Hand({Card('4', 'S'), Card('6', 'S'), Card('A', 'S')}),
        Hand({Card('A', 'S'), Card('9', 'S')}),
        Hand({Card('A', 'S'), Card('8', 'S')}),
        Hand({Card('A', 'S'), Card('7', 'S')}),
        Hand({Card('A', 'S'), Card('6', 'S')}),
        Hand({Card('A', 'S'), Card('5', 'S')}),
        Hand({Card('A', 'S'), Card('4', 'S')}),
        Hand({Card('A', 'S'), Card('3', 'S')}),
        Hand({Card('A', 'S'), Card('2', 'S')}), // split vals now
        Hand({Card('A', 'S'), Card('A', 'S')}),
        Hand({Card('K', 'S'), Card('K', 'S')}),
        Hand({Card('9', 'S'), Card('9', 'S')}),
        Hand({Card('8', 'S'), Card('8', 'S')}),
        Hand({Card('7', 'S'), Card('7', 'S')}),
        Hand({Card('6', 'S'), Card('6', 'S')}),
        Hand({Card('5', 'S'), Card('5', 'S')}),
        Hand({Card('4', 'S'), Card('4', 'S')}),
        Hand({Card('3', 'S'), Card('3', 'S')}),
        Hand({Card('2', 'S'), Card('2', 'S')})
    };

    int iterations = 40000;
    for (Hand& h : handsToSimulate) {
        std::vector<char> allowedChoices = determineAllowedChoices(h, r, players[0].getBalance(), players[0].getHands().size());

        startingCards.clear();

        for (int i = 0; i < h.getCardsCount(); ++i) {
            // loop through all cards of the hand
            std::string  key = "player" + std::to_string(i + 1);
            startingCards.emplace(key, h.getCard(i));

        }
        // assign correct starting card to the dealer too
        for (int i = 11; i >= 2; --i) {
            char c;
            if (i >= 2 and i <= 9) {
                c = '0' + i;
            }
            if (i == 10) {
                c = 'T';
            }
            if (i == 11) {
                c = 'A';
            }

            startingCards.insert_or_assign("dealer1", Card(c, 'S')); // assign when it doesnt exist, insert when it does
            std::string uniqueKey = makeUniqueKey(h.getValue(), 2, h.isSoft(), h.isSplittable(), startingCards.at("dealer1").getValue(), shoe);
            if (h.getValue() == 21) {
                uniqueKey = makeUniqueKey(h.getValue(), 2, true, h.isSplittable(), startingCards.at("dealer1").getValue(), shoe);
                // there is no hard 21 with 2 cards, only soft, so manually add entry
            }
            std::string uniqueKeyMultiCard = makeUniqueKey(h.getValue(), 3, h.isSoft(), h.isSplittable(), startingCards.at("dealer1").getValue(), shoe);
            // correct starting cards now assigned
            for (double HiLoCount = -5; HiLoCount <= 5; HiLoCount += 1.0) {

                for (char choice : allowedChoices) {

                    inputMap["choice"] = choice;
                    double totalHandWager = 0;
                    double totalHandWinnings = 0;

                    for (int i = 0; i < iterations; ++i) {
                        shoe.setHiLoTrueCount(HiLoCount);
                        gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, &cache); // play one game of blackjack

                        auto& hres = gr.playerResults["Nevio"];

                        if (not(gr.dealerHadBlackjack and r.peakAmericanStyle)) {
                            totalHandWager += std::stod(inputMap["numHands"]) * std::stod(inputMap["bet"]); // only count initial bets as wagers, exclude splits and doubles
                            for (handResult& h : hres.handResults) {
                                totalHandWinnings += h.handWinnings; // count both double and split winnings
                            }
                        }
                        if (gr.dealerHadBlackjack and r.peakAmericanStyle) { i--; } // redo the simulation round
                    }
                    double baseEV = totalHandWinnings / totalHandWager;
                    //std::cout << uniqueKey << " base ev for " << choice << ": " << baseEV <<" at count: " <<HiLoCount<< "\n";
                    otherMiniCache.emplace_back(baseEV, inputMap);
                }

                std::sort(otherMiniCache.begin(), otherMiniCache.end(),
                    [](const auto& a, const auto& b) {
                        return a.first > b.first;
                    });

                if (!otherMiniCache.empty() && r.lateSurrenderAllowed) {
                    const auto& bestPair = otherMiniCache.front();
                    if (bestPair.first < -0.5) { // lateSurrender has better EV than best choice
                        auto surrenderMap = bestPair.second;
                        surrenderMap["lateSurrender"] = "y";
                        surrenderMap["choice"] = "r"; // set to r so we know to remove for 3+ hands
                        otherMiniCache.emplace(otherMiniCache.begin(), -0.5, std::move(surrenderMap)); // add it as the first element
                        // if the otherwise best choice has an ev of less than -0.5, the surrenderMap is added.
                    }
                }
                

                // here we have all inputMaps at some count for some uniqueKey.
                // determine if best choice has switched compared to best choice at some earlier count. (this would be an index point).
                if (!deviationsVector.empty() and !otherMiniCache.empty()) {
                    // if entry exists, compare "choice" in input map, if it differs its an index point.
                    const auto& newestIndexpoint = deviationsVector.back();
                    if (newestIndexpoint.second.count("choice") and otherMiniCache.front().second.count("choice") and newestIndexpoint.second.at("choice") != otherMiniCache.front().second.at("choice")) {
                        // it differs meaning currenct count is index point
                        deviationsVector.emplace_back(HiLoCount, otherMiniCache.front().second);
                        std::cout <<uniqueKey<< "\nIndex point: " << HiLoCount << ", deviation choice: " << otherMiniCache.front().second["choice"]<<std::endl;
                    }
                }
                else {
                    // meaning its first run, set first index point to -1000 to ensure scope
                    deviationsVector.emplace_back(-1000, otherMiniCache.front().second);
                    //std::cout << "\nIndex point: -1000, deviation choice: " << otherMiniCache.front().second["choice"]<<std::endl;
                }
            }
            // looped through all counts and noted index points, add them to deviationsCache and start over
            deviationsCache[uniqueKey] = deviationsVector;
            deviationsCache[uniqueKeyMultiCard] = deviationsVector;
            deviationsVector.clear();
            otherMiniCache.clear();
            // add next entry to our strategy
            cache[uniqueKey] = strategy->at(uniqueKey);
            cache[uniqueKeyMultiCard] = strategy->at(uniqueKeyMultiCard);
        }
    }
    inputMap["insurance"] = "y";
    inputMap["evenMoney"] = "y";
    inputMap["choice"] = "f";
    deviationsVector.emplace_back(3.0, inputMap);
    deviationsCache["insurance;"] = deviationsVector;
    return deviationsCache;
}