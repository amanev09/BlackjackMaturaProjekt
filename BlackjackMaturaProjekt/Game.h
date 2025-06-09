#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <optional>

#include "Card.h"
#include "Hand.h"
#include "Shoe.h"
#include "Player.h"
#include "Dealer.h"
#include "GameRules.h"
#include "Outcomes.h"

gameResult blackJack(GameRules& r, std::vector<Player>& players, Dealer& d, Shoe& shoe, const std::unordered_map<std::string, std::string>* inputChoices = nullptr,
	std::unordered_map<std::string, Card>* startingCards = nullptr);
