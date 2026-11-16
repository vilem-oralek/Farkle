//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_DICE_HPP
#define FARKLE_DICE_HPP
#include <random>


class Dice {
private:
    int value;
    bool locked = false;
public:
    Dice();
    void roll(std::mt19937& rng);
    int getValue() const;
    void setValue(int v);
    bool isLocked() const;
    void setLocked(bool v);
};


#endif //FARKLE_DICE_HPP
