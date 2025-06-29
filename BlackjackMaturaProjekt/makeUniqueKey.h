#pragma once
#include "Shoe.h"
#include <string>

std::string makeUniqueKey(int playerVal, int cardAmount, bool isSoft, bool isSplittable, int dealerUpCard, Shoe& shoe);