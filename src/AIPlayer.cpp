//
// Created by Vilém Orálek on 16.11.2026.
//

#include "AIPlayer.hpp"
#include "Scorer.hpp"
#include "Game.hpp"
#include <iostream>

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
    if (Scorer::calculateScore(activeValues) > 0) {
        std::cout << "Hráč " << getName() << " vybral všechny dostupné kostky." << std::endl;
        return activeIndices;
    }
    const std::array<int, 7> counts = Scorer::countValues(activeValues);
    std::vector<int> chosenIndices;

    std::cout << "Hráč " << getName() << " vybral kostky č.:";
    for (int i = 0; i < dice.size(); i++) {
        if (!dice[i].isLocked()) {
            const int val = dice[i].getValue();
            if (val == 1 || val == 5 || counts[val] >= 3) {
                chosenIndices.push_back(i);
                std::cout << " " << (i + 1);
            }
        }
    }
    std::cout << std::endl;

    return chosenIndices;
}

bool AIPlayer::wantsToRollAgain(int turnScore, int remainingDiceCount) {
    if (turnScore < Game::ON_BOARD_THRESHOLD && !this->isOnBoard()) return true;
    if (remainingDiceCount == 0) remainingDiceCount = 6;
    if (remainingDiceCount > 4) return true;
    if (remainingDiceCount < minDiceToReroll || turnScore > bankThreshold) return false;
    return true;
}