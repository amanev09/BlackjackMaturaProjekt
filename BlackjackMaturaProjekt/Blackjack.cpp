#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>

#include "Game.h"
#include "makeUniqueKey.h"
#include "determineAllowedChoices.h"
#include "basicStrategy.h"
#include "strategySimulation.h"
int main()
{
    //std::cout << "Welcome to the blackjack simulator by Nevio Amato!" << std::endl;

    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> strategy =  basicStrategy();
    strategySimulation(&strategy);
    return 0;
}
