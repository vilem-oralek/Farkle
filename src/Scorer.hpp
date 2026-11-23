//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_SCORER_HPP
#define FARKLE_SCORER_HPP
#include <vector>
#include <array>

class Scorer {
public:
    static std::array<int, 7> countValues(const std::vector<int>& diceValues);
    static int calculateScore(const std::vector<int>& diceValues);
    static bool hasScoringMove(const std::vector<int>& activeDiceValues);
};


#endif //FARKLE_SCORER_HPP
