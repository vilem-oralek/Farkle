//
// Created by Vilém Orálek on 16.11.2026.
//

#ifndef FARKLE_AIPLAYER_HPP
#define FARKLE_AIPLAYER_HPP

#include "Player.hpp"

class AIPlayer : public Player {
private:
    int bankThreshold; // hranice kdy AI přestane riskovat
    int minDiceToReroll; // pokud zbývá méně volných kostek AI uloží body
public:
    explicit AIPlayer(const std::string& name, int bankThreshold = 350, int minDiceToReroll = 3);

    std::vector<int> chooseDice(const std::vector<Dice>& dice) override;
    bool wantsToRollAgain(int turnScore, int remainingDiceCount) override;
};


#endif //FARKLE_AIPLAYER_HPP
