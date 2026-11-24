//
// Created by Vilém Orálek on 23.11.2026.
//
#include "catch.hpp"

#include "../src/Scorer.hpp"
#include "../src/Dice.hpp"
#include "../src/HumanPlayer.hpp"
#include "../src/AIPlayer.hpp"

TEST_CASE("Scorer - zakladni jednotlive kostky", "[scorer]") {
    REQUIRE(Scorer::calculateScore({1}) == 100);
    REQUIRE(Scorer::calculateScore({5}) == 50);
    REQUIRE(Scorer::calculateScore({1, 5}) == 150);
    REQUIRE(Scorer::calculateScore({1, 1, 5, 5}) == 300);
}

TEST_CASE("Scorer - trojice a vice stejnych hodnot", "[scorer]") {
    REQUIRE(Scorer::calculateScore({1, 1, 1}) == 300);
    REQUIRE(Scorer::calculateScore({2, 2, 2}) == 200);
    REQUIRE(Scorer::calculateScore({3, 3, 3}) == 300);
    REQUIRE(Scorer::calculateScore({4, 4, 4}) == 400);
    REQUIRE(Scorer::calculateScore({5, 5, 5}) == 500);
    REQUIRE(Scorer::calculateScore({6, 6, 6}) == 600);

    // Ctverice, petice, sestice
    REQUIRE(Scorer::calculateScore({2, 2, 2, 2}) == 1000);
    REQUIRE(Scorer::calculateScore({4, 4, 4, 4, 4}) == 2000);
    REQUIRE(Scorer::calculateScore({6, 6, 6, 6, 6, 6}) == 3000);
}

TEST_CASE("Scorer - specialni sestikostkove kombinace", "[scorer]") {
    // Postupka 1-6 = 1500
    REQUIRE(Scorer::calculateScore({1, 2, 3, 4, 5, 6}) == 1500);
    REQUIRE(Scorer::calculateScore({6, 4, 2, 5, 1, 3}) == 1500);

    // Tri pary = 1500
    REQUIRE(Scorer::calculateScore({2, 2, 3, 3, 4, 4}) == 1500);
    REQUIRE(Scorer::calculateScore({1, 1, 5, 5, 6, 6}) == 1500);

    // Ctverice + dvojice = 1500
    REQUIRE(Scorer::calculateScore({2, 2, 2, 2, 3, 3}) == 1500);

    // Dve trojice = 2500
    REQUIRE(Scorer::calculateScore({2, 2, 2, 6, 6, 6}) == 2500);
    REQUIRE(Scorer::calculateScore({1, 1, 1, 5, 5, 5}) == 2500);
}

TEST_CASE("Scorer - neplatne kombinace a Farkle detekce", "[scorer]") {
    // Prazdny vyber nebo nebodujici kostky vraci <= 0
    REQUIRE(Scorer::calculateScore({}) <= 0);
    REQUIRE(Scorer::calculateScore({2, 3}) <= 0);
    REQUIRE(Scorer::calculateScore({1, 2}) <= 0); // 2 neboduje, cely vyber je neplatny

    // Farkle (zadny bodovany tah)
    REQUIRE_FALSE(Scorer::hasScoringMove({2, 3, 4, 6, 6, 2}));
    REQUIRE_FALSE(Scorer::hasScoringMove({2, 2, 3, 4, 6}));

    // Existuje bodovany tah
    REQUIRE(Scorer::hasScoringMove({2, 3, 4, 6, 6, 5}));
    REQUIRE(Scorer::hasScoringMove({2, 3, 4, 4, 4, 6}));
    REQUIRE(Scorer::hasScoringMove({1, 1, 3, 3, 5, 5}));
    REQUIRE(Scorer::hasScoringMove({1, 2, 3, 4, 5, 6}));

    // Hodnota mimo 1-6
    REQUIRE_THROWS_AS(Scorer::calculateScore({0, 1, 2}), std::invalid_argument);
}

TEST_CASE("Dice a Player - validace vstupu a vyjimky", "[model]") {
    Dice d;
    REQUIRE_NOTHROW(d.setValue(1));
    REQUIRE_NOTHROW(d.setValue(6));
    REQUIRE_THROWS_AS(d.setValue(0), std::invalid_argument);
    REQUIRE_THROWS_AS(d.setValue(7), std::invalid_argument);

    REQUIRE_NOTHROW(HumanPlayer("Vilem"));
    REQUIRE_THROWS_AS(HumanPlayer(""), std::invalid_argument);
    REQUIRE_THROWS_AS(HumanPlayer("VelmiDlouheJmenoPresDvacetZnaku"), std::invalid_argument);
}

TEST_CASE("Player - skore a vstup do hry (onBoard)", "[player]") {
    HumanPlayer p("Alice");
    REQUIRE(p.getName() == "Alice");
    REQUIRE(p.getTotalScore() == 0);
    REQUIRE_FALSE(p.isOnBoard());

    p.setOnBoard(true);
    REQUIRE(p.isOnBoard());

    p.addScore(650);
    REQUIRE(p.getTotalScore() == 650);

    p.setTotalScore(1000);
    REQUIRE(p.getTotalScore() == 1000);
}

TEST_CASE("Dice - zamykani a deterministicky hod", "[dice]") {
    Dice d;
    REQUIRE_FALSE(d.isLocked());
    d.setLocked(true);
    REQUIRE(d.isLocked());

    std::mt19937 rng1(42);
    std::mt19937 rng2(42);
    Dice d1, d2;
    for (int i = 0; i < 20; ++i) {
        d1.roll(rng1);
        d2.roll(rng2);
        REQUIRE(d1.getValue() == d2.getValue());
        REQUIRE(d1.getValue() >= 1);
        REQUIRE(d1.getValue() <= 6);
    }
}

TEST_CASE("AIPlayer - vyber kostek a rozhodovani o dalsim hodu", "[ai]") {
    // Opatrny AI: bankThreshold = 400, minDiceToReroll = 4
    AIPlayer cautiousAI("Bot1", 400, 4);

    // 1. Vsechny kostky boduji (postupka) -> vybere vsech 6
    std::vector<Dice> straightDice(6);
    for (int i = 0; i < 6; ++i) straightDice[i].setValue(i + 1);
    auto chosenAll = cautiousAI.chooseDice(straightDice);
    REQUIRE(chosenAll.size() == 6);

    // 2. Kombinace bodujicich a nebodujicich kostek: {1, 2, 3, 5, 4, 6} -> vybere indexy 0 (hodnota 1) a 3 (hodnota 5)
    std::vector<Dice> mixedDice(6);
    int vals[] = {1, 2, 3, 5, 2, 6};
    for (int i = 0; i < 6; ++i) mixedDice[i].setValue(vals[i]);
    auto chosenMixed = cautiousAI.chooseDice(mixedDice);
    REQUIRE(chosenMixed == std::vector<int>{0, 3});

    // 3. Rozhodovani o dalsim hodu
    // Neni na tabuli (isOnBoard == false) a ma < 500 bodu -> musi hazet dal
    REQUIRE(cautiousAI.wantsToRollAgain(300, 2));

    // Uz je na tabuli -> zastavi se, pokud ma > bankThreshold nebo < minDiceToReroll
    cautiousAI.setOnBoard(true);
    REQUIRE_FALSE(cautiousAI.wantsToRollAgain(450, 4));
    REQUIRE_FALSE(cautiousAI.wantsToRollAgain(200, 3));
    REQUIRE(cautiousAI.wantsToRollAgain(200, 5)); // Horke kostky / > 4 volne kostky -> hazi dal
}