//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_GAME_HPP
#define FARKLE_GAME_HPP


#include <memory>
#include <random>
#include <vector>
#include "Dice.hpp"
#include "Player.hpp"

class Game {
public:
    static constexpr int ON_BOARD_THRESHOLD = 500;
    static constexpr int WINNING_SCORE = 10000;
    static constexpr int DICE_COUNT = 6;

private:
    std::vector<std::unique_ptr<Player>> players;
    std::vector<Dice> dice;
    std::mt19937 rng;

    void rollUnlockedDice();
    void resetAllDice();
    int countUnlockedDice() const;
    std::vector<int> getUnlockedValues() const;

    bool isValidSelection(const std::vector<int>& selectedIndices) const;
    void playTurn(Player& player);

    void printDice() const;
    void printScoreboard() const;
    void announceWinner() const;

public:
    Game();

    void addPlayer(std::unique_ptr<Player> player);
    void run();
};

#endif //FARKLE_GAME_HPP
