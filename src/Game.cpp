//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Game.hpp"

#include <iostream>

#include "Scorer.hpp"
#include <array>

Game::Game() : rng(std::random_device{}()), dice(DICE_COUNT) {}

void Game::rollUnlockedDice() {
    for (int i = 0; i < DICE_COUNT; i++) {
        if (!dice[i].isLocked()) {
            dice[i].roll(rng);
        }
    }
}

void Game::resetAllDice() {
    for (int i = 0; i < DICE_COUNT; i++) {
        dice[i].setLocked(false);
    }
}

int Game::countUnlockedDice() const{
    int count = 0;
    for (int i = 0; i < DICE_COUNT; i++) {
        if (!dice[i].isLocked()) {
            count++;
        }
    }
    return count;
}

std::vector<int> Game::getUnlockedValues() const {
    std::vector<int> values;
    for (int i = 0; i < DICE_COUNT; i++) {
        if (!dice[i].isLocked()) {
            values.push_back(dice[i].getValue());
        }
    }
    return values;
}

bool Game::isValidSelection(const std::vector<int>& selectedIndices) const {
    std::array<bool, DICE_COUNT> seen{};
    std::vector<int> selectedValues;
    if (selectedIndices.size() > DICE_COUNT || selectedIndices.empty()) return false;
    for (int i = 0; i < selectedIndices.size(); i++) {
        if (selectedIndices[i] < 0 || selectedIndices[i] >= DICE_COUNT) return false;
        if (seen[selectedIndices[i]] == true) return false;
        seen[selectedIndices[i]] = true;
        if (dice[selectedIndices[i]].isLocked()) return false;
        selectedValues.push_back(dice[selectedIndices[i]].getValue());
    }
    if (Scorer::calculateScore(selectedValues) <= 0) return false;
    return true;
}

void Game::printDice() const {
    std::cout << std::endl;
    std::cout << "---KOSTKY---" << std::endl;
    for (int i = 0; i < DICE_COUNT; i++) {
        std::cout << "kostka č." << i + 1 << " [" << dice[i].getValue() << "] ";
        if (dice[i].isLocked()) std::cout << "uzamčená";
        std::cout << std::endl;
    }
}

void Game::printScoreboard() const {
    std::cout << std::endl;
    std::cout << "---SKÓRE HRÁČŮ--- " << std::endl;
    for (int i = 0; i < players.size(); i++) {
        std::cout << players[i]->getName() << ": " << players[i]->getTotalScore();
        if (!players[i]->isOnBoard()) std::cout << " - nesplněn vstup";
        std::cout << std::endl;
    }
}

void Game::announceWinner() const {
    std::cout << std::endl;
    int highest_score = 0;
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getTotalScore() > highest_score) {
            highest_score = players[i]->getTotalScore();
        }
    }
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getTotalScore() == highest_score) {
            std::cout <<"Hráč "<< players[i]->getName() << " zvítězil se skórem: "
            << players[i]->getTotalScore() << "." << std::endl;
        }
    }
}

void Game::playTurn(Player& player) {
    int turnScore = 0;
    int rollScore = 0;

    std::cout << std::endl <<"---Na tahu je " << player.getName()
          << " (Skóre: " << player.getTotalScore() << ")---" << std::endl;

    do {
        if (countUnlockedDice() == 0) {
            resetAllDice();
        }
        std::vector<int> chosenDice{};
        std::vector<int> chosenDiceValues{};
        rollUnlockedDice();
        printDice();
        if (!Scorer::hasScoringMove(getUnlockedValues())) {
            std::cout << "FARKLE! Hráč " << player.getName()
            << " spálil tah (přichází o všechny body z kola)." << std::endl;
            resetAllDice();
            return;
        }
        while (true) {
            chosenDice = player.chooseDice(dice);
            if (isValidSelection(chosenDice)) break;
            std::cout << "Neplatný výběr kostek, zkus to znovu." << std::endl;
        }
        for (int i = 0; i < chosenDice.size(); i++) {
            chosenDiceValues.push_back(dice[chosenDice[i]].getValue());
            dice[chosenDice[i]].setLocked(true);
        }
        rollScore = Scorer::calculateScore(chosenDiceValues);
        turnScore += rollScore;

        if (countUnlockedDice() == 0) {
            std::cout << std::endl << "HORKÉ KOSTKY!!! Všechny kostky bodovaly, můžeš házet znovu se všemi." << std::endl;
        }

        std::cout << "Hráč " << player.getName() << " v tomto hodu uhrál "
        << rollScore << " bodů, (v tomto kole celkem " << turnScore << " bodů)." << std::endl;

    }
    while (player.wantsToRollAgain(turnScore,countUnlockedDice()));

    if (player.isOnBoard()) {
        player.setTotalScore(player.getTotalScore() + turnScore);
        std::cout << std::endl << "Hráč " << player.getName() << " si přičítá "
        << turnScore << " bodů." << std::endl;
    }
    else if (!player.isOnBoard() && turnScore >= ON_BOARD_THRESHOLD) {
        player.setOnBoard(true);
        player.setTotalScore(player.getTotalScore() + turnScore);
        std::cout << std::endl << "Hráč " << player.getName() << " splnil vstupní limit a přičítá si "
        << turnScore << " bodů." << std::endl;
    }
    else{
        std::cout << std::endl << "Hráč " << player.getName()
        << " stále nesplnil vstupní limit, získává tedy 0 bodů." << std::endl;
    }
    resetAllDice();
}

void Game::addPlayer(std::unique_ptr<Player> player) {
    players.push_back(std::move(player));
}


void Game::run() {
    if (players.empty()) return;
    bool lastRound = false;
    bool end = false;
    int triggerIndex = 0;
    std::cout << std::endl << "---ZAČÁTEK HRY---" << std::endl;

    while (!end) {
        for (int i = 0; i < players.size(); i++) {
            if (lastRound && i == triggerIndex) {
                end = true;
                break;
            }
            playTurn(*players[i]);
            if (lastRound) continue;
            if (players[i]->getTotalScore() >= WINNING_SCORE) {
                lastRound = true;
                triggerIndex = i;
                std::cout << "Hráč " << players[i]->getName() << " přesáhl hranici " << WINNING_SCORE
                << " bodů. Ostatní hráči odehrají poslední kolo." << std::endl;
            }
        }
        if (!end) printScoreboard();
    }
    announceWinner();
}