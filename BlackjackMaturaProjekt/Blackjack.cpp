#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

#include "Game.h"


int main()
{
    std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::vector<Player> players;
    GameRules r;
    players.emplace_back("Nevio");
    Dealer d;
    Shoe shoe(r.deckCount);
    shoe.shuffle();
    std::unordered_map<std::string, std::string> inputMap = {
        {"numHands", "1"},
        {"bet", "1"},
        {"earlySurrender", "n"},
        {"evenMoney", "y"},
        {"insurance", "n"},
        {"lateSurrender", "n"},
        {"firstChoice", "s"}
    };
    std::unordered_map<std::string, Card> startingCards = {
        {"player1", Card('2', 'S')},
        {"player2", Card('2', 'S')},
        {"dealer1", Card('Q', 'S')}
        
    };
    double preBank = players[0].getBalance();
    int iterations = 1000;
    for (int i = 0; i < iterations; ++i) {
        blackJack(r, players, d, shoe, &inputMap, &startingCards);
    }
    double postBank = players[0].getBalance();
    std::cout << "EV: " << (postBank-preBank)/iterations;
    return 0;
}
