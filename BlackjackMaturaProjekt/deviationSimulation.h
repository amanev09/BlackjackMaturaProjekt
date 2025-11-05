#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <optional>

std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>> deviationStrategy(
	std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>* strategy);