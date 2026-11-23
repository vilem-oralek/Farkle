//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Dice.hpp"
#include <stdexcept>

Dice::Dice() : value(1) {}

void Dice::roll(std::mt19937& rng) {
    value = static_cast<int>(rng() % 6) + 1;
}

int Dice::getValue() const {
    return value;
}

void Dice::setValue(int v) {
    if (v < 1 || v > 6) throw std::invalid_argument("nevalidní hodnota kostky");
    value = v;
}

bool Dice::isLocked() const {
    return locked;
}

void Dice::setLocked(bool v) {
    locked = v;
}