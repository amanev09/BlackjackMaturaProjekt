#include "determineAllowedChoices.h"

std::vector<char> determineAllowedChoices(Hand& hand, GameRules& r, double balance, int playerHandSize) { 
	std::vector<char> allowedChoices;

    allowedChoices.push_back('s'); // these are always allowed
    if (not(hand.getFlagIsSplitHand() and hand.getCard(0).getRank() == 'A' and not r.hitSplitAcesAllowed)) {
        // the hand is a split resulting from a previous hand with 2 aces, only allow hitting when rules allow it
        allowedChoices.push_back('h');
    }
    // double allowed anytime on the first 2 cards of a hand. SO if a hand has exactly 2 cards, double down is allowed
    // additional check to ensure player has the balance necessary to double down
    // additional check with rules
    if (hand.getCardsCount() == 2 && balance >= hand.getBet()) {

        if (r.doubleDownAlwaysAllowed) {
            if (r.doubleAfterSplit and r.hitSplitAcesAllowed) { // doubling not allowed when hitting not allowed too even if DAS allowed
                // always allowed
                allowedChoices.push_back('d');
            }
            else if (not hand.getFlagIsSplitHand()) {
                // only allowed when the hand is NOT a split hand
                allowedChoices.push_back('d');
            }
        }
        else if (std::find(r.doubleDownOnCardValues.begin(), r.doubleDownOnCardValues.end(),
            hand.getValue()) != r.doubleDownOnCardValues.end()) { // searches the vector for the current hand value
            // if doubling is not always allowed, check if the current value is in the array of allowed double values.
            if (r.doubleAfterSplit and r.hitSplitAcesAllowed) {
                // always allowed
                allowedChoices.push_back('d');
            }
            else if (not hand.getFlagIsSplitHand()) {
                // only allowed when the hand is NOT a split hand
                allowedChoices.push_back('d');
            }
        }
        // also if we are on two cards, if cards are the same, splitting would also be allowed
        // check that the amount of hands the player has does not surpas the max split limit
        // if resplitting aces is not allowed, prohibit splitting when both cards are aces and the hand has flag isSplitHand
        if (hand.getCard(0).getValue() == hand.getCard(1).getValue() and playerHandSize <= r.maxSplitHands) {
            if (not(hand.getCard(0).getRank() == 'A' and hand.getFlagIsSplitHand() and not r.resplitAcesAllowed)) {
                // only when both cards are Aces and the hand is a split hand already and resplitting aces is prohibited will this not run
                allowedChoices.push_back('p');
            }
        }
    }

	return allowedChoices;
}