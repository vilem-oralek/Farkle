//
// Created by Vilém Orálek on 16.11.2026.
//

#include "HumanPlayer.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

HumanPlayer::HumanPlayer(const std::string& name) : Player(name) {}

std::vector<int> HumanPlayer::chooseDice(const std::vector<Dice>&) {
    std::vector<int> diceNumbers;
    std::string line;
    int x;
    std::cout << "Zadej čísla kostek k odložení (1-6, odděleně mezerou): ";
    std::getline(std::cin, line);
    std::stringstream ss(line);
    while (ss >> x) {
        diceNumbers.push_back(x-1);
    }
    ss.clear();
    ss >> std::ws;
    if (!ss.eof()) return {};
    return diceNumbers;
}

bool HumanPlayer::wantsToRollAgain(int turnScore, int remainingDiceCount) {
    char response;
    while (true) {
        if (remainingDiceCount == 0) remainingDiceCount = 6;
        std::cout << "Aktuální body v tahu: " << turnScore
                  << " | Volných kostek: " << remainingDiceCount
                  << ". Chceš házet znovu (h), nebo uložit skóre (s)? ";

        std::cin >> response;
        if (!std::cin) return false;
        std::cin.ignore(10000, '\n');

        if (response == 'h' || response == 'H') return true;
        if (response == 's' || response == 'S') return false;
        std::cout << "Neplatná volba, zadej 'h' nebo 's'.\n";
    }
}