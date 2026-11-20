//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_HUMANPLAYER_HPP
#define FARKLE_HUMANPLAYER_HPP

#include "Player.hpp"


class HumanPlayer : public Player {
public:
    explicit HumanPlayer(const std::string& name);

    std::vector<int> chooseDice(const std::vector<Dice>& dice) override;
    bool wantsToRollAgain(int turnScore, int remainingDiceCount) override;
};


#endif //FARKLE_HUMANPLAYER_HPP
