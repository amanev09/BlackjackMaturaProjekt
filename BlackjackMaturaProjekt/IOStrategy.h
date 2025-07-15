#pragma once
#include "unordered_map"
#include "string"
#include "json.hpp"
#include <fstream>


using json = nlohmann::json;

void saveStrategy(
    const std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>& strategy,
    const std::string& filename
);

void loadStrategy(
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>& strategy,
    const std::string& filename
);