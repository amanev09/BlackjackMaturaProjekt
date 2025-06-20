#include <string>
#include "Shoe.h"
#include <sstream> 
#include "makeUniqueKey.h"
std::string makeUniqueKey(int playerVal, bool isSoft, bool isSplittable, int dealerUpCard, Shoe& shoe) {
	// assumption is that gamerules stay the same during a 
	std::ostringstream key;
	key << playerVal<<";" << isSoft << ";" << isSplittable << ";" << dealerUpCard << ";";
	//key << shoe.shoeKey(); //optional for better accuracy
	return key.str();
}