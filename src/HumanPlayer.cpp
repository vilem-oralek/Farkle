//
// Created by Vilém Orálek on 16.11.2026.
//

#include "HumanPlayer.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

HumanPlayer::HumanPlayer(const std::string& name) : Player(name) {}

std::vector<int> HumanPlayer::chooseDice(const std::vector<Dice>& dice) {
    std::vector<int> diceNumbers;
    std::string line;
    int x;
    std::cout << "Zadej čísla kostek k odložení (1-6, odděleně mezerou): ";
    std::getline(std::cin, line);
    std::stringstream ss(line);
    while (ss >> x) {
        diceNumbers.push_back(x-1);
    }
    return diceNumbers;
}

bool HumanPlayer::wantsToRollAgain(int turnScore, int remainingDiceCount) {
    std::string response;
    while (true) {
        if (remainingDiceCount == 0) remainingDiceCount = 6;
        std::cout << "Aktuální body v tahu: " << turnScore
                  << " | Volných kostek: " << remainingDiceCount
                  << ". Chceš házet znovu (h), nebo uložit skóre (s)? ";
        std::getline(std::cin, response);
        if (response == "h" || response == "H") return true;
        if (response == "s" || response == "S") return false;
        std::cout << "Neplatna volba, zadej 'h' nebo 's'.\n";
    }
}