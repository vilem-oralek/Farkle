//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Player.hpp"

Player::Player(const std::string& name) : playerName(name) {}

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