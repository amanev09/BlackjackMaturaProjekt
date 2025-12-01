#pragma once
#include <unordered_map>
#include <string>

void strategyToBasicStrategyChart(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache);
void strategyToBasicStrategyEVChart(std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> cache);