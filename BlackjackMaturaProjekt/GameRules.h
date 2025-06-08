#pragma once

struct GameRules {
    int deckCount = 2;
    int maxSplitHands = 1000000; // infinite
    double blackjackPayoutRatio = 3.0 / 2.0;
    bool doubleAfterSplit = true;
    bool soft17 = false; // false means h17
    bool peakAmericanStyle = true; // otherwise european style, dealer doesnt peak at all until after all player actions are over
    bool earlySurrenderAllowed = false;
    bool lateSurrenderAllowed = false;
    bool insuranceAllowed = true;
    bool evenMoneyAllowed = true;
    bool resplitAcesAllowed = true;
    bool hitSplitAcesAllowed = true;
    bool doubleDownAlwaysAllowed = true;
    std::vector<int> doubleDownOnCardValues; // only needed when doubleDownAlwaysAllowed = false
    double minBet = 0;
    double maxBet = 100;
    GameRules() {
        
    }
};