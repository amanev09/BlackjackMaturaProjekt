#pragma once
// want to make all static to acces gamerules without needing instance, then do proper ENHC, fix other bugs in gamerules
struct GameRules {
    int deckCount = 6;
	int maxSplitHands = 2;  // max hands to split to. 1 means no splitting at all, 3 would mean maximum of 3 hands (2 splits)
    double blackjackPayoutRatio = 3.0 / 2.0;
    bool doubleAfterSplit = true;
    bool hitSoft17 = false;
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = false;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = false; 
    bool hitSplitAcesAllowed = false;
    bool doubleDownAlwaysAllowed = true;
    std::vector<int> doubleDownOnCardValues = {11,10,9}; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
    GameRules() {
        
    }
};