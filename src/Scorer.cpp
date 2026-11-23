//
// Created by Vilém Orálek on 16.11.2026.
//

#include "Scorer.hpp"
#include <stdexcept>

std::array<int, 7> Scorer::countValues(const std::vector<int>& diceValues) {
    std::array<int, 7> result = {0};
    for (int i = 0; i < diceValues.size(); i++) {
        if (diceValues[i] < 1 || diceValues[i] > 6) {
            throw std::invalid_argument("nevalidní hodnota kostky");
        }
        result[diceValues[i]]++;
    }
    return result;
}

int Scorer::calculateScore(const std::vector<int>& diceValues) {
    if (diceValues.empty()) return -1;
    std::array<int, 7> counts = countValues(diceValues);
    int score = 0;



    for (int i = 1; i < counts.size(); i++) {
        // 6 of a kind
        if (counts[i] == 6) return 3000;

        // 2 triplets
        if (counts[i] == 3) {
            for (int j = i + 1; j < counts.size(); j++) {
                if (counts[j] == 3) return 2500;
            }
        }
        // 5 of a kind
        if (counts[i] == 5) {
            score += 2000;
            counts[i] = 0;
        }

        // 4 of a kind & a pair
        if (counts[i] == 4) {
            for (int j = 1; j < counts.size(); j++) {
                if (counts[j] == 2) return 1500;
            }
        }

        // 3 pairs
        if (counts[i] == 2) {
            for (int j = i + 1; j < counts.size(); j++) {
                if (counts[j] == 2) {
                    for (int k = j + 1; k < counts.size(); k++) {
                        if (counts[k] == 2) return 1500;
                    }
                }
            }
        }

        // 1-6 straight
        if (counts[i] == 1) {
            bool straight = true;
            for (int j = 1; j < counts.size(); j++) {
                if (counts[j] != 1) {
                    straight = false;
                }
            }
            if (straight) return 1500;
        }

        // 4 of a kind
        if (counts[i] == 4) {
            score += 1000;
            counts[i] = 0;
        }

        // 3 of a kind
        if (counts[i] == 3) {
            if (i == 1) score += 300;
            else score += i * 100;
            counts[i] = 0;
        }
    }

    // 5s and 1s
    score += counts[1] * 100 + counts[5] * 50;
    counts[1] = 0;
    counts[5] = 0;

    for (int i = 1; i < counts.size(); i++) {
        if (counts[i] > 0) {
            return -1;
        }
    }
    return score;
}

bool Scorer::hasScoringMove(const std::vector<int>& activeDiceValues) {
    std::array<int, 7> counts = countValues(activeDiceValues);
    int pairs = 0;

    if (counts[1] > 0 || counts[5] > 0) return true;
    for (int i = 1; i < counts.size(); i++) {
        if (counts[i] >= 3) return true;
        if (counts[i] == 2) pairs++;
    }
    if (pairs == 3) return true;
    return false;
}