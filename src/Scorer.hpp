//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_SCORER_HPP
#define FARKLE_SCORER_HPP
#include <vector>


class Scorer {
private:
    std::array<int, 7> countValues(const std::vector<int>& diceValues);
public:
    int calculateScore(const std::vector<int>& diceValues);
    bool hasScoringMove(const std::vector<int>& activeDiceValues);
};


#endif //FARKLE_SCORER_HPP
