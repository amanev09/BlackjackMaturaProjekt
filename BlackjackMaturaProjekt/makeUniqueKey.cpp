#include <string>
#include "Shoe.h"
#include <sstream> 
#include "makeUniqueKey.h"
std::string makeUniqueKey(int playerVal, int cardAmount, bool isSoft, bool isSplittable, int dealerUpCard, Shoe& shoe) {
	// assumption is that gamerules stay the same during a game
	std::string c;
	if (cardAmount == 2) { c = "2"; } else { c = "3+"; }
	std::ostringstream key;
	key << playerVal<<";" << c << ";" << isSoft << ";" << isSplittable << ";" << dealerUpCard << ";";
	//key << shoe.shoeKey(); //optional for better accuracy
	return key.str();
}