//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Dice.hpp"
#include <stdexcept>

Dice::Dice() : value(1) {}

void Dice::roll(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 6);
    value = dist(rng);
}

int Dice::getValue() const {
    return value;
}

void Dice::setValue(int v) {
    if (v < 1 || v > 6) throw std::invalid_argument("invalid value for Dice");
    value = v;
}

bool Dice::isLocked() const {
    return locked;
}

void Dice::setLocked(bool v) {
    locked = v;
}