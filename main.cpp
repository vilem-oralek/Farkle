#include <iostream>
#include <string>

#include "src/Game.hpp"
#include "src/AIPlayer.hpp"
#include "src/HumanPlayer.hpp"


void printHelp() {
    std::cout << "\n=== PRAVIDLA HRY FARKLE ===\n"
              << "1. PRŮBĚH TAHU:\n"
              << "   - Hráč na začátku tahu hází všemi " << Game::DICE_COUNT << " kostkami.\n"
              << "   - Po každém hodu musí hráč odložit (uzamknout) alespoň jednu bodovanou kombinaci.\n"
              << "   - Poté se může rozhodnout, zda tah ukončí a zapíše si nahrané body,\n"
              << "     nebo bude pokračovat v házení se zbývajícími neuzamčenými kostkami.\n\n"
              << "2. FARKLE A HORKÉ KOSTKY:\n"
              << "   - FARKLE: Pokud po hodu žádná z neuzamčených kostek netvoří bodovanou kombinaci,\n"
              << "     hráč okamžitě končí tah a přichází o všechny body nahrané v tomto kole.\n"
              << "   - HORKÉ KOSTKY: Pokud hráč postupně využije k bodování všech " << Game::DICE_COUNT << " kostek,\n"
              << "     všechny kostky se znovu odemknou a hráč může pokračovat v házení se všemi\n\n"
              << "3. VSTUP DO HRY A VÍTĚZSTVÍ:\n"
              << "   - Vstupní limit: Hráč se musí nejprve dostat 'na tabuli' tím, že v rámci\n"
              << "     jednoho kola nahraje alespoň " << Game::ON_BOARD_THRESHOLD << " bodů. Do té doby se mu body nezapisují.\n"
              << "   - Konec hry: Jakmile některý z hráčů dosáhne alespoň " << Game::WINNING_SCORE << " bodů,\n"
              << "     spustí se poslední kolo, ve kterém mají všichni ostatní hráči ještě jeden tah.\n"
              << "   - Vítězí hráč s nejvyšším celkovým skóre po dohrání posledního kola.\n\n"
              << "4. BODOVÁNÍ KOMBINACÍ (v rámci jednoho hodu):\n"
              << "   - Samostatná 1            : 100 bodů\n"
              << "   - Samostatná 5            : 50 bodů\n"
              << "   - Trojice stejných čísel  : 100 × hodnota kostky (trojice jedniček = 300 bodů)\n"
              << "   - Čtveřice stejných čísel : 1 000 bodů\n"
              << "   - Pětice stejných čísel   : 2 000 bodů\n"
              << "   - Šestice stejných čísel  : 3 000 bodů\n"
              << "   - Postupka (1-2-3-4-5-6)  : 1 500 bodů\n"
              << "   - Tři páry                : 1 500 bodů\n"
              << "   - Čtveřice a dvojice      : 1 500 bodů\n"
              << "   - Dvě trojice             : 2 500 bodů\n";
}

int main(int argc, char* argv[]) {

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
    }

    Game game;

    char response = 'a';
    int playerCount = 0;

    while (true) {
        std::cout << "Přejete si přidat nového hráče? (a/n)" << std::endl;
        std:: cin >> response;
        if (response == 'a' || response == 'A') {

            char type;
            std::string name;

            std::cout << "Zadejte jméno hráče" << std::endl;
            std::getline(std::cin >> std::ws, name);

            std::cout << "Je to člověk nebo AI? (c/a): ";
            std::cin >> type;

            if (type == 'a' || type == 'A') {
                char difficulty = '2';
                std::cout << "Obtížnost AI (1 = opatrný, 2 = vyvážený, 3 = riskující): ";
                std::cin >> difficulty;

                if (difficulty == '1') {
                    game.addPlayer(std::make_unique<AIPlayer>(name, 300, 3));
                } else if (difficulty == '3') {
                    game.addPlayer(std::make_unique<AIPlayer>(name, 1000, 2));
                } else {
                    game.addPlayer(std::make_unique<AIPlayer>(name, 500, 2));
                    std::cout << "Neplatná volba, automaticky zvolena obtížnost 2 (vyvážený)." << std::endl;
                }
            } else game.addPlayer(std::make_unique<HumanPlayer>(name));
            playerCount++;
        }
        else if (response == 'n' || response == 'N') {
            if (playerCount < 2) {
                std::cout << "Hra musí obsahovat alespoň 2 hráče." << std::endl;
            }
            else {
                game.run();
                break;
            }
        }
        else std::cout << "Špatný vstup, zkuste znovu." << std::endl;
    }

    return 0;
}