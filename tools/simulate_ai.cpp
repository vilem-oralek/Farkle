// Pomocný nástroj pro porovnání úrovní AI (NENÍ součástí CMake ani spouštěné hry).
//
// Pouští partie AI proti AI přes veřejné rozhraní tříd Game a AIPlayer a ze
// vypsaného textu si vytahuje statistiky (výhry, remízy, Farkle, body na tah).
// Každá partie má vlastní seed (základní seed + číslo partie), takže jsou
// výsledky přesně reprodukovatelné. Začínající hráč se v partiích střídá.
//
// Překlad (z kořene projektu, jeden příkaz na jednom řádku):
//   g++ -std=c++14 -O2 tools/simulate_ai.cpp src/Game.cpp src/Dice.cpp src/Player.cpp src/AIPlayer.cpp src/HumanPlayer.cpp src/Scorer.cpp -o simulate_ai
//
// Použití:
//   ./simulate_ai pair <bankA> <minA> <bankB> <minB> <počet_partií> [základní_seed]
//   ./simulate_ai grid <počet_partií_na_dvojici> [základní_seed]

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../src/AIPlayer.hpp"
#include "../src/Game.hpp"

namespace {

struct Config {
    int bank;     // bankThreshold
    int minDice;  // minDiceToReroll
};

// Zachytává výstup hry a ze řádků vytahuje statistiky pro hráče "A" a "B".
class OutputParser : public std::streambuf {
public:
    long turns[2] = {0, 0};
    long farkles[2] = {0, 0};
    int finalScore[2] = {0, 0};
    bool tie = false;

    void resetGame() {
        finalScore[0] = finalScore[1] = 0;
        tie = false;
        afterEnd_ = false;
        line_.clear();
    }

protected:
    int overflow(int c) override {
        if (c == EOF) return c;
        if (c == '\n') {
            processLine();
            line_.clear();
        } else {
            line_.push_back(static_cast<char>(c));
        }
        return c;
    }
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        for (std::streamsize i = 0; i < n; ++i) overflow(static_cast<unsigned char>(s[i]));
        return n;
    }

private:
    std::string line_;
    bool afterEnd_ = false;

    void processLine() {
        if (line_.find("=== KONEC HRY ===") != std::string::npos) afterEnd_ = true;
        for (int p = 0; p < 2; ++p) {
            const std::string name(1, p == 0 ? 'A' : 'B');
            if (line_.find("Na tahu je " + name + " (") != std::string::npos) {
                ++turns[p];
            } else if (line_.find("FARKLE! Hráč " + name + " ") != std::string::npos) {
                ++farkles[p];
            } else if (afterEnd_ && line_.compare(0, 3, name + ": ") == 0) {
                finalScore[p] = std::atoi(line_.c_str() + 3);
            }
        }
        if (line_.find("remízovali") != std::string::npos) tie = true;
    }
};

struct MatchResult {
    long winsA = 0, winsB = 0, ties = 0, games = 0;
    long turnsA = 0, turnsB = 0, farklesA = 0, farklesB = 0;
    long pointsA = 0, pointsB = 0;
};

long g_totalGames = 0;

MatchResult playMatch(const Config& a, const Config& b, int games, unsigned baseSeed) {
    MatchResult r;
    OutputParser parser;
    std::streambuf* original = std::cout.rdbuf(&parser);

    for (int g = 0; g < games; ++g) {
        parser.resetGame();
        Game game(baseSeed + static_cast<unsigned>(g));
        if (g % 2 == 0) {
            game.addPlayer(std::make_unique<AIPlayer>("A", a.bank, a.minDice));
            game.addPlayer(std::make_unique<AIPlayer>("B", b.bank, b.minDice));
        } else {
            game.addPlayer(std::make_unique<AIPlayer>("B", b.bank, b.minDice));
            game.addPlayer(std::make_unique<AIPlayer>("A", a.bank, a.minDice));
        }
        game.run();

        if (parser.finalScore[0] > parser.finalScore[1]) ++r.winsA;
        else if (parser.finalScore[0] < parser.finalScore[1]) ++r.winsB;
        else ++r.ties;
        r.pointsA += parser.finalScore[0];
        r.pointsB += parser.finalScore[1];
        ++r.games;
        ++g_totalGames;
    }
    std::cout.rdbuf(original);
    r.turnsA = parser.turns[0];
    r.turnsB = parser.turns[1];
    r.farklesA = parser.farkles[0];
    r.farklesB = parser.farkles[1];
    return r;
}

int runPair(int argc, char* argv[]) {
    if (argc < 7) {
        std::cerr << "Použití: simulate_ai pair <bankA> <minA> <bankB> <minB> <partie> [seed]\n";
        return 1;
    }
    Config a{std::atoi(argv[2]), std::atoi(argv[3])};
    Config b{std::atoi(argv[4]), std::atoi(argv[5])};
    int games = std::atoi(argv[6]);
    unsigned seed = argc > 7 ? static_cast<unsigned>(std::strtoul(argv[7], nullptr, 10)) : 1u;

    MatchResult r = playMatch(a, b, games, seed);
    double n = static_cast<double>(r.games);
    std::printf("A(%d,%d) vs B(%d,%d), %ld partií\n", a.bank, a.minDice, b.bank, b.minDice, r.games);
    std::printf("  výhry A: %.1f %% | remízy: %.2f %% | výhry B: %.1f %%\n",
                100.0 * r.winsA / n, 100.0 * r.ties / n, 100.0 * r.winsB / n);
    std::printf("  Farkle na tah: A %.1f %%, B %.1f %%\n",
                100.0 * r.farklesA / r.turnsA, 100.0 * r.farklesB / r.turnsB);
    std::printf("  body na tah:   A %.0f, B %.0f | tahů na hráče v partii: %.1f\n",
                static_cast<double>(r.pointsA) / r.turnsA, static_cast<double>(r.pointsB) / r.turnsB,
                static_cast<double>(r.turnsA) / n);
    return 0;
}

int runGrid(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Použití: simulate_ai grid <partie_na_dvojici> [seed]\n";
        return 1;
    }
    int games = std::atoi(argv[2]);
    unsigned seed = argc > 3 ? static_cast<unsigned>(std::strtoul(argv[3], nullptr, 10)) : 1u;

    const int banks[] = {250, 300, 350, 400, 450, 500, 600, 750, 1000, 1500, 2500};
    const int minDice[] = {1, 2, 3, 4, 5};
    std::vector<Config> cfg;
    for (int b : banks)
        for (int m : minDice) cfg.push_back({b, m});

    std::vector<double> score(cfg.size(), 0.0);
    for (size_t i = 0; i < cfg.size(); ++i) {
        for (size_t j = i + 1; j < cfg.size(); ++j) {
            MatchResult r = playMatch(cfg[i], cfg[j], games, seed);
            double n = static_cast<double>(r.games);
            score[i] += (r.winsA + 0.5 * r.ties) / n;
            score[j] += (r.winsB + 0.5 * r.ties) / n;
        }
    }

    std::vector<size_t> order(cfg.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](size_t x, size_t y) { return score[x] > score[y]; });

    std::printf("Celkem %ld partií, %zu konfigurací, %d partií na dvojici\n", g_totalGames, cfg.size(), games);
    std::printf("poř. | bankThreshold | minDiceToReroll | průměrná úspěšnost proti ostatním\n");
    for (size_t k = 0; k < order.size(); ++k) {
        const Config& c = cfg[order[k]];
        std::printf("%4zu | %13d | %15d | %.1f %%\n", k + 1, c.bank, c.minDice,
                    100.0 * score[order[k]] / (cfg.size() - 1));
    }
    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Použití: simulate_ai pair|grid ...\n";
        return 1;
    }
    std::string mode = argv[1];
    if (mode == "pair") return runPair(argc, argv);
    if (mode == "grid") return runGrid(argc, argv);
    std::cerr << "Neznámý režim: " << mode << "\n";
    return 1;
}
