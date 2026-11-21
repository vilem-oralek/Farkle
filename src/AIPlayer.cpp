//
// Created by Vilém Orálek on 16.11.2026.
//

#include "AIPlayer.hpp"
#include "Scorer.hpp"

AIPlayer::AIPlayer(const std::string& name, int bankThreshold, int minDiceToReroll)
    : Player(name), bankThreshold(bankThreshold), minDiceToReroll(minDiceToReroll){}

std::vector<int> AIPlayer::chooseDice(const std::vector<Dice>& dice) {
    std::vector<int> activeValues;
    std::vector<int> activeIndices;

    for (int i = 0; i < dice.size(); i++) {
        if (!dice[i].isLocked()) {
            activeValues.push_back(dice[i].getValue());
            activeIndices.push_back(i);
        }
    }
    if (Scorer::calculateScore(activeValues) > 0) return activeIndices;

    std::array<int, 7> counts = Scorer::countValues(activeValues);
    std::vector<int> chosenIndices;

    for (int i = 0; i < dice.size(); i++) {
        if (!dice[i].isLocked()) {
            int val = dice[i].getValue();
            if (val == 1 || val == 5 || counts[val] >= 3) {
                chosenIndices.push_back(i);
            }
        }
    }
    return chosenIndices;
}

bool AIPlayer::wantsToRollAgain(int turnScore, int remainingDiceCount) {
    if (turnScore < 500 && !this->isOnBoard()) return true;
    if (remainingDiceCount == 0) remainingDiceCount = 6;
    if (remainingDiceCount > 4) return true;
    if (remainingDiceCount < minDiceToReroll || turnScore > bankThreshold) return false;
    return true;
}