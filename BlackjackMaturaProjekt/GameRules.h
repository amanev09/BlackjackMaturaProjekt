#pragma once

struct GameRules {
    int deckCount = 1;
    int maxSplitHands = 1000000; // infinite fix
    double blackjackPayoutRatio = 3.0 / 2.0;
    bool doubleAfterSplit = true;
    bool hitSoft17 = false;
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = false;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = true; // fix
    bool hitSplitAcesAllowed = true;
    bool doubleDownAlwaysAllowed = true;
    std::vector<int> doubleDownOnCardValues; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
    GameRules() {
        
    }
};