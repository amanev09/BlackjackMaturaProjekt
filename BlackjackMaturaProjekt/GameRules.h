#pragma once
// want to make all static to acces gamerules without needing instance, then do proper ENHC, fix other bugs in gamerules
struct GameRules {
    int deckCount = 8;
    int maxSplitHands = 4; 
    double blackjackPayoutRatio = 3.0 / 2.0;
    bool doubleAfterSplit = true;
    bool hitSoft17 = false;
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = true;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = true; 
    bool hitSplitAcesAllowed = true;
    bool doubleDownAlwaysAllowed = true;
    std::vector<int> doubleDownOnCardValues = {11,10,9}; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
    GameRules() {
        
    }
};