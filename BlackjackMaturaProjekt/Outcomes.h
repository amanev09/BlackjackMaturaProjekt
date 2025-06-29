#pragma once
#include <unordered_map>
#include <string>

enum class HandOutcome : int {
    blackjackWin = 1,
    evenMoney = 2,
    wonByDealerBust = 3,
    wonByHigherValue = 4,
    pushByDealerBlackjack = 5,
    pushBySameValue = 6,
    lostByLowerValue = 7,
    lostByBust = 8,
    surrendered = 9
};

enum class InsuranceOutcome : int {
    NotTaken,
    TakenLost,
    TakenWon
};

struct handResult {
    // this struct contains the result data for each hand after a game
    HandOutcome handOutcome; // holds the resolution data for each hand (how did we lose/win) *this is a must set*
    InsuranceOutcome insuranceOutcome = InsuranceOutcome::NotTaken; // holds resolution data for insurance, by default set on no insurance taken
};

struct playerResult {
    std::vector<handResult> handResults;
};

struct gameResult {
    std::unordered_map<std::string, playerResult> playerResults;
    bool dealerHadBlackjack = false; // if the dealer had a blackjack(21 with 2 cards) that means that players couldn't even make a choice(except insurance/even money). This is needed for basic strategy
    // EV calculations because we need to discard all rounds where this is set to true or otherwise or EV values get skewed into the negative
};