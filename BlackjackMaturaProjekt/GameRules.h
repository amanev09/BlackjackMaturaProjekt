#pragma once

struct GameRules {
    int deckCount = 8;
    int maxSplitHands = 1000; // infinite fix
    double blackjackPayoutRatio = 3.0 / 2.0;
    bool doubleAfterSplit = true;
    bool hitSoft17 = true;
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = false;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = true; 
    bool hitSplitAcesAllowed = false;
    bool doubleDownAlwaysAllowed = true;
    std::vector<int> doubleDownOnCardValues = {11,10,9}; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
    GameRules() {
        
    }
};