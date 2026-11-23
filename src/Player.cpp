//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Player.hpp"
#include <stdexcept>

Player::Player(const std::string& name) : playerName(name) {
    if (name.empty() || name.length() > MAX_NAME_LENGTH) {
        throw std::invalid_argument("Jméno hráče musí mít 1 až " + std::to_string(MAX_NAME_LENGTH) + " znaků.");
    }
}

void Player::setName(const std::string& name) {
    playerName = name;
}

std::string Player::getName() const {
    return playerName;
}

void Player::setOnBoard(bool b) {
    onBoard = b;
}

bool Player::isOnBoard() const {
    return onBoard;
}

void Player::addScore(int score) {
    totalScore += score;
}

void Player::setTotalScore(int score) {
    totalScore = score;
}

int Player::getTotalScore() const {
    return totalScore;
}