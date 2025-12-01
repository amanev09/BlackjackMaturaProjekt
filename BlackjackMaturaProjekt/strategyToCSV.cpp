//converts basic strategy from ram to csv file in multiple ways
#include "strategyToCSV.h"
#include "makeUniqueKey.h"
#include "Shoe.h"
#include <fstream>

void strategyToBasicStrategyChart(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache) {

	Shoe shoe(1); // shoe object needed for makeUniqueKey, no real purpose

    std::ofstream basicStrategyFile("basicStrategy.csv");
    basicStrategyFile << "HardTotals,2,3,4,5,6,7,8,9,T,A\n";
    for (int i = 20; i >= 5; --i) {
        basicStrategyFile << i;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, false, j, shoe);
            if (cache.count(k)) {
                std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
                if (c == "d" && cache.at(k)[1].second.at("choice") == "s") { c = "ds"; } // second best choice important too when double isnt allowed
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
                if (c == "d" && cache.at(k)[1].second.at("choice") == "s") { c = "ds"; } // second best choice important too when double isnt allowed
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
            if (c == "d" && cache.at(k)[1].second.at("choice") == "s") { c = "ds"; } // second best choice important too when double isnt allowed
            basicStrategyFile << c;
        }
        basicStrategyFile << ",";
    }
    basicStrategyFile << "\n";
    for (int i = 20; i >= 4; i -= 2) {
        basicStrategyFile << i / 2 << ";" << i / 2;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, true, j, shoe);
            if (cache.count(k)) {
                std::string c = cache.at(k)[0].second.at("choice"); // get basic strategy choice
                if (c == "d" && cache.at(k)[1].second.at("choice") == "s") { c = "ds"; } // second best choice important too when double isnt allowed
                basicStrategyFile << "," << c;
            }
        }
        basicStrategyFile << "\n";
    }


}

void strategyToBasicStrategyEVChart(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache) {

    Shoe shoe(1); // shoe object needed for makeUniqueKey, no real purpose

    std::ofstream basicStrategyFile("basicStrategyEVChart.csv");
    basicStrategyFile << "HardTotals,2,3,4,5,6,7,8,9,T,A\n";
    for (int i = 20; i >= 5; --i) {
        basicStrategyFile << i;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, false, j, shoe);
            if (cache.count(k)) {
                double c = cache.at(k)[0].first; // get basic strategy choice
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
                double c = cache.at(k)[0].first; // get basic strategy choice
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
            double c = cache.at(k)[0].first; // get basic strategy choice
            basicStrategyFile << c;
        }
        basicStrategyFile << ",";
    }
    basicStrategyFile << "\n";
    for (int i = 20; i >= 4; i -= 2) {
        basicStrategyFile << i / 2 << ";" << i / 2;
        for (int j = 2; j <= 11; ++j) {
            std::string k = makeUniqueKey(i, 2, false, true, j, shoe);
            if (cache.count(k)) {
                double c = cache.at(k)[0].first; // get basic strategy choice
                basicStrategyFile << "," << c;
            }
        }
        basicStrategyFile << "\n";
    }


}