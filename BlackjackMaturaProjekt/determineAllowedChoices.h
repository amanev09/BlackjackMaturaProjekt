#pragma once
#include <vector>
#include "Hand.h"
#include "GameRules.h"

std::vector<char> determineAllowedChoices(Hand& hand, GameRules& r, double balance, int playerHandSize);