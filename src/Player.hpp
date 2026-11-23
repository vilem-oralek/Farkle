//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_PLAYER_HPP
#define FARKLE_PLAYER_HPP
#include <string>
#include <vector>
#include "Dice.hpp"
#include <cstddef>


class Player {
public:
    static constexpr size_t MAX_NAME_LENGTH = 20;
private:
    std::string playerName;
    bool onBoard = false;
    int totalScore = 0;
public:
    explicit Player(const std::string& name);
    virtual ~Player() = default;
    void setName(const std::string& name);
    std::string getName() const;
    void setOnBoard(bool b);
    bool isOnBoard() const;
    void addScore(int score);
    void setTotalScore(int score);
    int getTotalScore() const;
    virtual std::vector<int> chooseDice(const std::vector<Dice>& dice) = 0;
    virtual bool wantsToRollAgain(int turnScore, int remainingDiceCount) = 0;
};


#endif //FARKLE_PLAYER_HPP
