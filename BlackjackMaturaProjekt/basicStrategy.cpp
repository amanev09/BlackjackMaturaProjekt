// calculates basic strategy
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"
#include "basicStrategy.h"
std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> basicStrategy() {

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
        {"insurance", "y"},
        {"lateSurrender", "n"},
        {"choice", "s"}
    };
    std::unordered_map<std::string, Card> startingCards = { // empty because loop overrides it
        {"player1", Card('7', 'S')},
        {"player2", Card('K', 'S')},
        {"dealer1", Card('A', 'S')},
    };

    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache; // gameState, corresponding inputMap, used by simulation
    std::map<std::string, std::vector<std::pair<double, std::string>>> largeCache; // gameState, corresponding EVs for all decisions, used for output.
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

    int iterations = 100000;
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
            std::vector<std::pair<double, std::string>> miniCache; // EV values for given situationkey
            std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> otherMiniCache;
            double totalInsuranceWinnings = 0;
            double totalInsuranceWager = 0;
            for (char choice : allowedChoices) {
                inputMap["choice"] = choice;
                
                double totalHandWager = 0;
                double totalHandWinnings = 0;

                for (int i = 0; i < iterations; ++i) {
                    shoe.reset();
                    // optionally modify deck here
                    //shoe.removeCard('2', 'S');
                    gameResult gr = blackJack(r, players, d, shoe, &inputMap, &startingCards, &cache); // play one game of blackjack

                    auto& hres = gr.playerResults["Nevio"];
                    
                    if (!hres.handResults.empty() and hres.handResults[0].insuranceOutcome != InsuranceOutcome::NotTaken) {
                        // assumption is that if we simulate with multiple starter hands, that all hands either take or dont take insurance
                        totalInsuranceWager += std::stod(inputMap["numHands"]) * std::stod(inputMap["bet"]) / 2;
                        totalInsuranceWinnings += hres.handResults[0].insuranceWinnings * std::stod(inputMap["numHands"]); // winnings same on all hands since bet is same
                    }

                    if (not(gr.dealerHadBlackjack and r.peakAmericanStyle)) {
                        totalHandWager += std::stod(inputMap["numHands"]) * std::stod(inputMap["bet"]); // only count initial bets as wagers, exclude splits and doubles
                        for (handResult& h : hres.handResults) {
                            totalHandWinnings += h.handWinnings; // count both double and split winnings
                        }
                    }
                    if (gr.dealerHadBlackjack and r.peakAmericanStyle) { i--; } // redo the simulation round
                }
                double baseEV = totalHandWinnings / totalHandWager;
                std::cout << uniqueKey << "base ev for "<<choice<<": " << baseEV << "\n";
                miniCache.emplace_back(baseEV, std::string(1, choice));
                otherMiniCache.emplace_back(baseEV, inputMap);
            }
            double insuranceEV = totalInsuranceWager > 0 ? totalInsuranceWinnings / totalInsuranceWager : 0.0;
            std::cout << uniqueKey << "insurance EV: " << insuranceEV << "\n";
            // determine choice of inputMap to be saved inside cache
            std::sort(miniCache.begin(), miniCache.end(),
                [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });
            miniCache.emplace_back(insuranceEV, "insurance");

            std::sort(otherMiniCache.begin(), otherMiniCache.end(),
                [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });
            for (auto& pair : otherMiniCache) {
                pair.second["insurance"] = insuranceEV > 0 ? "y" : "n";
            }
            // optionally calculate surrender map(ev always -0.5)
            cache[uniqueKey] = otherMiniCache;
            largeCache[uniqueKey] = miniCache;

            otherMiniCache.erase(
                std::remove_if(
                    otherMiniCache.begin(),
                    otherMiniCache.end(),
                    [](const auto& entry) {
                        const auto& choice = entry.second.at("choice");
                        return choice == "d" || choice == "p";
                    }
                ),
                otherMiniCache.end()
            );

            miniCache.erase(
                std::remove_if(
                    miniCache.begin(),
                    miniCache.end(),
                    [](const std::pair<double, std::string>& entry) {
                        return entry.second == "d" || entry.second == "p";
                    }
                ),
                miniCache.end()
            );


            cache[uniqueKeyMultiCard] = otherMiniCache;
            largeCache[uniqueKeyMultiCard] = miniCache;
        }
    }
    for (const auto& mapEntry : largeCache) {
        const std::string& key = mapEntry.first;
        const std::vector<std::pair<double, std::string>>& vec = mapEntry.second;

        //std::cout << "Key: " << key;

        //std::cout << "  EV: " << vec[0].first << ", Choice: " << vec[0].second << "\n";
    }
    return cache;
}