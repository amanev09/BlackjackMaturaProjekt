#include "IOStrategy.h"

void saveStrategy(
    const std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>& strategy,
    const std::string& filename
) {
    json j;

    for (const auto& [key, vec] : strategy) {
        for (const auto& [ev, inputMap] : vec) {
            j[key].push_back({
                {"ev", ev},
                {"inputMap", inputMap}
                });
        }
    }

    std::ofstream out(filename);
    out << j.dump(2); // 2 = pretty print with 2 spaces
}

void loadStrategy(
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>>& strategy,
    const std::string& filename
) {
    std::ifstream in(filename);
    if (!in) return; // skip if file doesn't exist

    json j;
    in >> j;

    strategy.clear();
    for (auto& [key, entries] : j.items()) {
        for (auto& entry : entries) {
            double ev = entry["ev"];
            std::unordered_map<std::string, std::string> inputMap = entry["inputMap"].get<std::unordered_map<std::string, std::string>>();
            strategy[key].emplace_back(ev, inputMap);
        }
    }
}
