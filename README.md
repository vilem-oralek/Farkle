# Farkle – semestrální práce (C++14)

| | |
|---|---|
| **Autor** | Vilém Orálek |
| **Hra** | Farkle (kostková hra) |
| **Jazyk / standard** | C++14, CMake 3.10 a novější |
| **Ovládání** | výhradně v terminálu, bez externích knihoven (kromě Catch2 pro jednotkové testy) |
| **Datum** | listopad 2026 |

Program umožňuje libovolnou kombinaci lidských a počítačových hráčů (minimálně dva hráči nutností). Zadání „člověk proti počítači“ je splněno a navíc lze hrát člověk proti člověku a počítač proti počítači (využito v testech).

## Obsah

1. [Popis hry a herních strategií](#1-popis-hry-a-herních-strategií)
2. [Postup řešení](#2-postup-řešení)
3. [Překlad a spuštění](#3-překlad-a-spuštění)
4. [Přepínače (argumenty) programu](#4-přepínače-argumenty-programu)
5. [Ovládání aplikace](#5-ovládání-aplikace)
6. [Testování](#6-testování)
7. [Videa a časový harmonogram](#7-videa-a-časový-harmonogram)
8. [Použité zdroje a využití AI](#8-použité-zdroje-a-využití-ai)

---

## 1. Popis hry a herních strategií

### 1.1 Pravidla hry

Farkle je kostková hra, ve které se hráči střídají v tazích. Hráč v tahu hází šesti kostkami a snaží se nasbírat co nejvíc bodů, aniž by hodil takzvaný **Farkle** (hod, ve kterém žádná kostka nebodovala).

**Průběh tahu**

1. **Hod.** Hráč hodí všemi neuzamčenými kostkami (na začátku tahu všemi šesti).
2. **Výběr.** Musí odložit (uzamknout) alespoň jednu bodovanou kombinaci. Vybrané kostky se vyhodnocují **jako celek**: pokud mezi nimi je kostka, která nebodovala, je celý výběr neplatný. **POZOR!** vybrané kostky z hodu se nevyhodnocují s již uzamčenými kostkami z předchozích hodů.
3. **Rozhodnutí.** Hráč buď hodí znovu zbývajícími kostkami (`h`), nebo nasbírané body uloží (`s`) a tah předá.
4. **Farkle.** Pokud po hodu žádná neuzamčená kostka nebodovala, tah okamžitě končí a hráč **přichází o všechny body nasbírané v tomto tahu**.
5. **Horké kostky.** Pokud hráč uzamkne všech šest kostek, všechny se znovu odemknou a hráč může házet znovu všemi.
6. **Vstupní limit.** Dokud hráč není „ve hře", musí v **jednom tahu** nasbírat alespoň **500 bodů**. Pokud tah ukončí s méně body, získává 0 bodů.
7. **Konec hry.** Jakmile některý hráč dosáhne alespoň **10 000 bodů**, spustí se poslední kolo, ve kterém má každý ostatní hráč ještě **jeden tah**. Vítězí hráč s nejvyšším celkovým skóre. Při shodném nejvyšším skóre je výsledek **remíza**.

**Bodování**

| Kombinace | Body |
|---|---|
| Samostatná 1 | 100 |
| Samostatná 5 | 50 |
| Trojice jedniček | 300 |
| Trojice 2, 3, 4, 5, 6 | 100 × hodnota kostky (200, 300, 400, 500, 600) |
| Čtveřice stejných hodnot | 1 000 |
| Pětice stejných hodnot | 2 000 |
| Šestice stejných hodnot | 3 000 |
| Postupka 1-2-3-4-5-6 | 1 500 |
| Tři páry | 1 500 |
| Čtveřice a dvojice | 1 500 |
| Dvě trojice | 2 500 |

Příklad: výběr hodnot `1 1 5` je hodnocen jako 100 + 100 + 50 = 250 bodů, výběr `1 1 1` jako 300 bodů (ne 3 × 100) a výběr `1 1 1 1` jako 1 000 bodů.

### 1.2 Herní strategie

Průběh hry je dán hody kostkami, proto strategie ovlivňuje pouze to, **kdy hráč riskuje a kdy ukládá**. Klíčový údaj je pravděpodobnost Farkle podle počtu volných kostek. Hodnoty jsou přesně spočítané úplným výčtem všech hodů podle pravidel použitých v programu (`Scorer::hasScoringMove`):

| Volných kostek | 6 | 5 | 4 | 3 | 2 | 1 |
|---|---|---|---|---|---|---|
| Pravděpodobnost Farkle | 2,31 % | 7,72 % | 15,74 % | 27,78 % | 44,44 % | 66,67 % |

#### Výherní strategie

1. **Nejdříve se dostat do hry.** Dokud hráč nesplnil vstupní limit, nemá smysl tah ukládat. Hráč proto hází dál, dokud nemá v jednom tahu **alespoň 500 bodů**, a teprve potom je uloží. (Počítačový hráč to dělá automaticky.)
2. **Poté hrát „logicky“.** Po splnění vstupního limitu vážit zisk proti riziku Farkle podle tabulky výše: s pěti nebo šesti volnými kostkami je riziko malé a vyplatí se házet. Se dvěma nebo třemi kostkami je riziko vysoké, takže má smysl ukládat, zvlášť když má hráč v tahu hodně bodů.
3. **Využít horké kostky.** Když se podaří uzamknout všech šest kostek, hráč dostane znovu šest kostek a riziko Farkle klesne na 2,31 % -> má smysl házet znovu.

#### Strategie vedoucí k prohře

Nejjistější prohra je nikdy nesplnit vstupní limit:

- Hráč po prvním hodu odloží nejméně hodnocenou kombinaci (např. jednu pětku za 50 bodů) a okamžitě tah uloží. Protože 50 < 500, program mu body nezapíše (výpis „stále nesplnil vstupní limit, získává tedy 0 bodů“). Tak může pokračovat donekonečna.
- Nebo opačně: hází stále znovu, dokud nepadne Farkle, a tím přijde o všechny body z tahu.
- Kombinace obou přístupů: hráč odkládá vždy nejmenší možnou kombinaci a když mu padne vysoká kombinace (např. šest šestek), **neukládá a hází dál až do Farkle**, aby se nedostal do hry.

#### Strategie vedoucí k remíze

Remíza nastane jen při **přesně shodném** celkovém skóre po posledním kole (skóre jsou násobky 50), takže je z velké části **věcí náhody**. V simulacích AI proti AI (viz kapitola 1.3) nastala přibližně v 0,5 % partií, tedy zhruba jednou za dvě stě her. Strategie, která šanci zvyšuje:

- Zůstat **těsně pod hranicí 10 000 bodů** a v dalších tazích raději házet až do Farkle, než hranici překročit. Poslední kolo pak spustí soupeř a hráč má **poslední tah** s přesnou znalostí cílového skóre.
- Jakmile soupeř hranici 10 000 přeskočí, snažit se jeho skóre **vyrovnat**: hráč tah uloží přesně ve chvíli, kdy se jeho součet rovná soupeřovu skóre.
- Počítačoví hráči tuto taktiku nepoužívají: ukládají podle pevných pravidel a hranici 10 000 překročí, jakmile na ni dosáhnou.
- Pokud tuto taktiku používají oba hráči, hra se může protahovat, protože pravidla neomezují počet kol.

### 1.3 Strategie počítačových hráčů a vyvážení obtížností (simulace)

#### Jak AI rozhoduje

`AIPlayer` se při každém tahu rozhoduje dvakrát:

- **Výběr kostek (`chooseDice`)** je chamtivý: pokud všechny neuzamčené kostky body dávají, odloží je všechny. Jinak odloží všechny jedničky, všechny pětky a každou hodnotu, která padla alespoň třikrát.
- **Další hod (`wantsToRollAgain`)** se řídí čtyřmi pravidly v tomto pořadí:
  1. Pokud hráč není ve hře a má v tahu méně než 500 bodů, **hází dál**.
  2. Pokud jsou všechny kostky uzamčené (horké kostky), počítá se, jako by byly volné všech šest.
  3. Pokud zbývá **více než 4** volných kostek, **hází dál** (riziko Farkle je nejvýš 7,72 %).
  4. Jinak **uloží**, pokud zbývá méně než `minDiceToReroll` kostek **nebo** má v tahu víc než `bankThreshold` bodů. Jinak hází dál.

Úroveň obtížnosti je tedy dána dvěma čísly:

| Úroveň | Název | `bankThreshold` | `minDiceToReroll` | Chování |
|---|---|---|---|---|
| 1 | opatrný | 400 | 4 | uloží, jakmile zbývají 3 nebo méně kostek; se 4 kostkami hází, jen když má v tahu nejvýš 400 bodů |
| 2 | vyvážený | 500 | 3 | hází i se 3 kostkami; se 3 až 4 kostkami uloží, když má v tahu víc než 500 bodů |
| 3 | riskující | 500 | 2 | hází i se 2 kostkami; se 2 až 4 kostkami uloží, když má v tahu víc než 500 bodů |

#### Jak byly hodnoty zvoleny

Hodnoty nebyly odhadnuty, ale **změřeny simulacemi**. Asistent Claude (viz kapitola 8) na žádost autora odsimuloval celkem **přibližně 1,9 milionu partií AI proti AI** (průzkumná fáze asi 1,1 milionu partií a finální reprodukovatelná série 800 000 partií). Ve finální sérii hrály dvě AI proti sobě a v partiích se střídalo, kdo začíná. Každá partie měla vlastní seed, takže výsledky jdou přesně zopakovat.

**Měření 1: mřížka 55 konfigurací.** Každá kombinace `bankThreshold` ∈ {250, 300, 350, 400, 450, 500, 600, 750, 1000, 1500, 2500} a `minDiceToReroll` ∈ {1, 2, 3, 4, 5} hrála proti všem ostatním (1 485 dvojic × 200 partií = 297 000 partií). Průměrná úspěšnost podle `minDiceToReroll`:

| `minDiceToReroll` | Úspěšnost proti ostatním | Pořadí z 55 |
|---|---|---|
| 1 | 3,2 až 56,9 % | 4 až 55 |
| 2 | 34,0 až 56,5 % | 8 až 52 |
| **3** | **55,8 až 58,4 %** | **1 až 13** |
| 4 | 53,4 až 55,1 % | 16 až 27 |
| 5 | 46,8 až 48,1 % | 37 až 49 |

Zjištění: **rozhoduje `minDiceToReroll`**. Hodnota 3 je nejsilnější bez ohledu na `bankThreshold`, který mění výsledek jen o pár procentních bodů. Je to logické, protože u více než 4 kostek AI vždy hází, takže `bankThreshold` se uplatní jen při 1 až 4 volných kostkách.

**Měření 2: finální úrovně proti sobě** (100 000 partií na dvojici):

| Zápas | Vyhrává první | Remíza | Vyhrává druhý | Farkle na tah | Body na tah |
|---|---|---|---|---|---|
| opatrný vs vyvážený | 46,4 % | 0,59 % | 53,0 % | 13,3 % : 23,0 % | 486 : 498 |
| opatrný vs riskující | 49,6 % | 0,48 % | 49,9 % | 13,3 % : 38,1 % | 484 : 482 |
| vyvážený vs riskující | 53,0 % | 0,47 % | 46,6 % | 23,0 % : 38,1 % | 498 : 482 |

Statistická chyba je u 100 000 partií asi ±0,3 procentního bodu, u mřížky (asi 10 800 partií na konfiguraci) asi ±1 procentní bod (95% interval).

**Výsledek.** Všechny tři úrovně jsou dostatečně silné (žádná nevyhrává ani neprohrává výrazně) a **liší se mírou rizika**: počet Farkle na tah je 13 %, 23 % a 38 %. Názvy tedy odpovídají chování. „Vyvážený“ je v mřížce na 10. místě z 55 (56,4 %). Nejlepší nalezená konfigurace (`bankThreshold` 1500, `minDiceToReroll` 3) je o dva procentní body silnější, ale chová se prakticky stejně rizikově.

**Původní hodnoty a proč byly změněny:**

| Původní nastavení | Zjištění |
|---|---|
| „opatrný“ (300, 3) | 21,9 % Farkle na tah, tedy stejné riziko jako „vyvážený“ (23,0 %); mezi sebou hrají vyrovnaně (50,1 : 49,3). Název neodpovídal chování. |
| „vyvážený“ (500, 2) | 38,2 % Farkle na tah, ve skutečnosti riskující hra. |
| „riskující“ (1000, 2) | 45,5 % Farkle na tah, proti finálnímu „vyváženému“ jen 40,4 : 59,2; v mřížce 35. místo z 55. Příliš slabý. |

---

## 2. Postup řešení

### 2.1 Pořadí implementace

Projekt byl implementován metodou **zdola nahoru**: od nejmenších nezávislých stavebních kamenů až po celkovou orchestraci hry. Každou vrstvu bylo možné testovat izolovaně. Pořadí odpovídá historii gitu:

| Krok | Třída / část | Poznámka |
|---|---|---|
| 1 | základní struktura souborů | |
| 2 | `Dice` | jedna kostka, hod a uzamčení |
| 3 | `Scorer` | bodování bez stavu, snadno testovatelné |
| 4 | `Player` | abstraktní základ hráče |
| 5 | `HumanPlayer` | vstup z terminálu |
| 6 | `AIPlayer` | rozhodování AI hráče |
| 7 | `Game` | herní smyčka |
| 8 | `main.cpp` | argumenty a vytváření hráčů |
| 9 | opravy nekonzistencí, lepší hlášky | |
| 10 | jednotkové testy (Catch2) a testovací vstupy | |

### 2.2 Struktura projektu

```
Farkle/
├── main.cpp              celkové řízení: argumenty, nastavení hráčů, spuštění hry
├── CMakeLists.txt        překlad (cíle Farkle a farkle_tests)
├── README.md             tato dokumentace
├── src/
│   ├── Dice.hpp/.cpp         kostka
│   ├── Scorer.hpp/.cpp       bodování (statické metody)
│   ├── Player.hpp/.cpp       abstraktní hráč
│   ├── HumanPlayer.hpp/.cpp  člověk
│   ├── AIPlayer.hpp/.cpp     počítač
│   └── Game.hpp/.cpp         herní smyčka
└── tests/
    ├── test_farkle.cpp       jednotkové testy
    ├── catch.hpp, catch.cpp  Catch2 v2.13.10
    └── inputs/               vzorové vstupy pro scénáře s přesměrovaným vstupem

```

### 2.3 Diagram tříd

```
 main.cpp
    |  vytvoří hráče a předá je hře
    v
 +--------------------------------------------+
 | Game                                       |         +-----------------------+
 |  - players : vector<unique_ptr<Player>>    |-------->| Scorer (statické)     |
 |  - dice    : vector<Dice>  (6 kusů)        | používá | countValues           |
 |  - rng     : mt19937                       |         | calculateScore        |
 |  + addPlayer(), run()                      |         | hasScoringMove        |
 +--------------------------------------------+         +-----------------------+
       | 2..N                    | 6
       v                         v
 +-----------------------+   +------------------+
 | Player  (abstraktní)  |   | Dice             |
 |  - jméno, skóre,      |   |  - value 1..6    |
 |    onBoard            |   |  - locked        |
 |  + chooseDice()    =0 |   |  + roll(rng)     |
 |  + wantsToRollAgain=0 |   +------------------+
 +-----------------------+
       ^              ^
       |              |
 +-------------+  +-------------------------+
 | HumanPlayer |  | AIPlayer                |
 | čte std::cin|  |  - bankThreshold        |
 +-------------+  |  - minDiceToReroll      |
                  +-------------------------+
```

`Game` pracuje s hráči **polymorfně** přes `Player*` a nerozlišuje, zda je hráč člověk, nebo počítač. Vyhodnocení konce hry a vyhlášení vítěze nebo remízy je proto pro oba druhy stejné.

### 2.4 Popis tříd a metod

#### `Dice` (`src/Dice.hpp/.cpp`)

Jedna hrací kostka: hodnota 1 až 6 a příznak uzamčení.

| Metoda | Popis |
|---|---|
| `Dice()` | hodnota 1, kostka odemčená |
| `void roll(std::mt19937& rng)` | nastaví hodnotu `rng() % 6 + 1`. Generátor se předává **referencí**, aby všechny kostky sdílely jeden stav (při kopii by každá hodila totéž číslo). Místo `std::uniform_int_distribution` se používá modulo, protože standard přesně definuje výstup `mt19937`, ale ne algoritmus distribuce. Stejný seed dává stejné hody na každém kompilátoru. |
| `int getValue() const` | vrátí aktuální hodnotu |
| `void setValue(int v)` | nastaví hodnotu, pro `v` mimo 1 až 6 vyhodí `std::invalid_argument` (využívají testy) |
| `bool isLocked() const`, `void setLocked(bool)` | čtení a nastavení uzamčení |

#### `Scorer` (`src/Scorer.hpp/.cpp`)

Třída bez stavu, jen statické metody. Je oddělená od hry. Lze tedy testovat samostatně.

| Metoda | Popis |
|---|---|
| `std::array<int,7> countValues(const std::vector<int>&)` | spočítá výskyty hodnot (index 1 až 6); pro hodnotu mimo 1 až 6 vyhodí `std::invalid_argument` |
| `int calculateScore(const std::vector<int>&)` | vyhodnotí výběr **jako celek**. Postupně rozpozná šestici (3 000), dvě trojice (2 500), pětici (2 000), čtveřici s dvojicí (1 500), tři páry (1 500), postupku (1 500), čtveřici (1 000) a trojici; zbývající 1 a 5 přičte po 100 a 50. Vrací **-1**, pokud je výběr prázdný nebo obsahuje kostku, která nebodovala. |
| `bool hasScoringMove(const std::vector<int>&)` | vrátí `true`, pokud mezi kostkami existuje bodovaný tah (alespoň jedna 1 nebo 5, nějaká hodnota alespoň třikrát, nebo tři páry). Slouží k detekci Farkle. |

#### `Player` (`src/Player.hpp/.cpp`)

Abstraktní bázová třída: společný stav a rozhraní hráče.

| Metoda | Popis |
|---|---|
| `Player(const std::string& name)` | uloží jméno; pro prázdné jméno nebo jméno delší než `MAX_NAME_LENGTH` (20) vyhodí `std::invalid_argument` |
| `getName()`, `getTotalScore()`, `isOnBoard()` | čtení stavu |
| `setOnBoard(bool)`, `addScore(int)`, `setTotalScore(int)` | změna stavu |
| `virtual std::vector<int> chooseDice(const std::vector<Dice>&) = 0` | vrátí indexy (od 0) kostek, které hráč odkládá |
| `virtual bool wantsToRollAgain(int turnScore, int remainingDiceCount) = 0` | vrátí `true`, pokud chce hráč házet znovu, `false`, pokud ukládá |

#### `HumanPlayer` (`src/HumanPlayer.hpp/.cpp`)

| Metoda | Popis |
|---|---|
| `chooseDice` | načte celý řádek, rozloží ho přes `std::stringstream` na čísla a převede je z číslování 1 až 6 na indexy 0 až 5. Pokud řádek obsahuje cokoli nečíselného (např. `1,4` nebo `1 4 abc`), vrátí prázdný výběr, který `Game` odmítne. Při konci vstupu vrátí prázdný výběr a vstupní proud zůstane v chybovém stavu. |
| `wantsToRollAgain` | vypíše body v tahu a počet volných kostek (po horkých kostkách 6) a čte `h` nebo `s` (malé i velké) tak dlouho, dokud odpověď není platná. Při konci vstupu vrátí `false`. |

#### `AIPlayer` (`src/AIPlayer.hpp/.cpp`)

Parametry `bankThreshold` a `minDiceToReroll` se zadávají v konstruktoru `AIPlayer(name, bankThreshold, minDiceToReroll)`. Algoritmus je popsán v kapitole 1.3.

| Metoda | Popis |
|---|---|
| `chooseDice` | pokud všechny neuzamčené kostky bodují, vybere je všechny; jinak vybere všechny 1, 5 a hodnoty s alespoň třemi výskyty. Vypíše, které kostky vybral. |
| `wantsToRollAgain` | čtyři pravidla popsaná v kapitole 1.3 |

#### `Game` (`src/Game.hpp/.cpp`)

Řídí hru. Konstanty: `ON_BOARD_THRESHOLD = 500`, `WINNING_SCORE = 10000`, `DICE_COUNT = 6`.

| Metoda | Popis |
|---|---|
| `Game()` | seed generátoru z `std::random_device` |
| `explicit Game(unsigned int seed)` | pevný seed (přepínač `--seed`) |
| `void addPlayer(std::unique_ptr<Player>)` | přidá hráče do hry |
| `void run()` | hlavní smyčka: střídá hráče po kolech; po dosažení 10 000 bodů zapamatuje hráče, který hranici překročil, a nechá všechny ostatní odehrát jeden tah; poté vypíše skóre a vyhlásí vítěze. Při konci vstupu vypíše hlášku a skončí. |
| `void playTurn(Player&)` | jeden tah (algoritmus níže) |
| `bool isValidSelection(const std::vector<int>&) const` | výběr je platný, pokud není prázdný, všechny indexy jsou v rozsahu 0 až 5, nejsou duplicitní, kostky nejsou uzamčené a výběr podle `Scorer::calculateScore` bodovaný |
| `rollUnlockedDice()`, `resetAllDice()`, `countUnlockedDice()`, `getUnlockedValues()` | pomocné metody nad kostkami |
| `printDice()`, `printScoreboard()` | výpis kostek a tabulky skóre |
| `announceWinner() const` | najde nejvyšší skóre; pokud ho má jeden hráč, vypíše vítěze, jinak vypíše remízu všech hráčů s tímto skóre |

**Algoritmus tahu (`playTurn`)**

```
turnScore = 0
opakuj:
    pokud nejsou žádné volné kostky:  odemkni všechny            (nastane když horké kostky)
    hoď volnými kostkami a vypiš je
    pokud neexistuje bodovaný tah:    vypiš FARKLE, odemkni, KONEC TAHU (body z tahu propadají)
    opakuj, dokud není výběr platný:
        výběr = hráč.chooseDice(kostky)
    uzamkni vybrané kostky, turnScore += body výběru
    pokud nejsou žádné volné kostky:  vypiš HORKÉ KOSTKY
dokud hráč.wantsToRollAgain(turnScore, počet volných kostek)

pokud je hráč ve hře:                  skóre += turnScore
jinak pokud turnScore >= 500:             hráč splnil vstupní limit a je tímto ve hře, skóre += turnScore
jinak:                                    hráč získává 0 bodů
odemkni všechny kostky
```

#### `main.cpp`

| Část | Popis |
|---|---|
| `printHelp()` | vypíše nápovědu k přepínačům, ovládání a pravidlům |
| zpracování argumentů | `--help` / `-h` a `--seed <N>` (viz kapitola 4) |
| vytvoření hráčů | dotazuje se na hráče, dokud uživatel nezvolí `n` a nejsou alespoň dva vytvoření: jméno, typ (`c` člověk / `a` AI) a u AI obtížnost. Úroveň 1 vytvoří `AIPlayer(jméno, 400, 4)`, úroveň 2 `AIPlayer(jméno, 500, 3)`, úroveň 3 `AIPlayer(jméno, 500, 2)`. |
| spuštění | `game.run()` |

### 2.5 Návrhová rozhodnutí

- **Oddělení pravidel od hry.** `Scorer` nemá stav a nezná `Game`, takže jde testovat rychlými jednotkovými testy bez simulace partie.
- **Polymorfismus.** `Game` drží `std::vector<std::unique_ptr<Player>>` a nezáleží jí na tom, jestli hráč čte z klávesnice, nebo rozhoduje předurčeně.
- **Jeden generátor.** `Game` vlastní `std::mt19937` a předává ho kostkám referencí. Díky seedu jsou partie reprodukovatelné.
- **Bezpečné ukončení při konci vstupu.** Všechna místa, která čtou z `std::cin`, kontrolují stav proudu. Program se při konci vstupu (Ctrl+D, vyčerpaný soubor) nezacyklí, vypíše hlášku a skončí s kódem 0.

### 2.6 Známá omezení

- Délka jména se kontroluje v **bajtech**. České znaky s diakritikou zabírají 2 bajty, takže jméno s 11 a více takovými znaky (22 bajtů) se odmítne.
- **Neznámé přepínače se ignorují.** Hodnota `--seed` se čte funkcí `std::stoul`: nečíselný text je chyba, ale záporné číslo přeteče (tudíž je validní) a text za číslem (`12abc`) se ignoruje (text před číslem vyhodí výjimku).
- Neexistuje příkaz pro předčasné ukončení partie; použije se Ctrl+C nebo konec vstupu (Ctrl+D).
- Tah nemá časový limit.
- Program nekontroluje duplicitní jména hráčů (lze je ale rozeznat podle skóre).
- AI vždy odkládá všechny bodované kostky a `bankThreshold` ignoruje při 5 a 6 volných kostkách (záměr, riziko Farkle je tam nejvýš 7,72 %).
- Vstupní soubory s koncem řádku CRLF (Windows) vloží znak `\r` do jména hráče. Testovací vstupy mají konce řádků LF.

---

## 3. Překlad a spuštění

**Požadavky:** překladač podporující C++14 a CMake 3.10 nebo novější. Program nemá externí závislosti. Autor program vyvíjel na macOS (CLion, CMake 4.3.1); stejné výstupy byly ověřeny i na Linuxu (Ubuntu 24.04, g++ 13.3) pomocí AI asistenta na žádost autora.

**Překlad pomocí CMake (z kořene projektu):**

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Vzniknou dvě spustitelné aplikace:

| Soubor | Význam |
|---|---|
| `Farkle` | hra |
| `farkle_tests` | jednotkové testy (Catch2) |

Pokud jsou potřeba jen hra a ne testy, stačí `cmake --build . --target Farkle`.

**Překlad bez CMake** (jen hra):

```bash
g++ -std=c++14 -o Farkle main.cpp src/*.cpp
```

**Spuštění:**

```bash
./Farkle                       # hra s náhodným seedem
./Farkle --seed 42             # reprodukovatelná hra
./Farkle --help                # nápověda
./farkle_tests                 # jednotkové testy
```

V dalších kapitolách se příkazy spouštějí z kořene projektu, tedy `./build/Farkle` a `./build/farkle_tests`.

---

## 4. Přepínače (argumenty) programu

| Přepínač | Význam |
|---|---|
| `-h`, `--help` | vypíše nápovědu (možnosti, ovládání a pravidla hry) a program skončí s kódem 0 |
| `--seed <N>` | nastaví seed generátoru náhodných čísel; stejný seed a stejný vstup dají vždy stejnou partii (využito v testech) |

Argumenty se zpracovávají **zleva doprava**. Příklady a chování:

| Příkaz | Výsledek |
|---|---|
| `./Farkle --help` | vypíše nápovědu, kód 0 |
| `./Farkle -h` | totéž |
| `./Farkle --seed 42` | spustí hru s pevným seedem |
| `./Farkle --seed` | `Chyba: Přepínač --seed vyžaduje číselnou hodnotu.`, kód 1 |
| `./Farkle --seed abc` | `Chyba: Neplatná hodnota pro --seed (zadejte celé číslo).`, kód 1 |
| `./Farkle --help --seed abc` | vypíše nápovědu (přepínač `--help` se zpracuje první), kód 0 |
| `./Farkle --seed abc --help` | chyba u `--seed`, kód 1 |

Ukončovací kódy: **0** při běžném konci, nápovědě a konci vstupu, **1** při chybě argumentu.

---

## 5. Ovládání aplikace

### 5.1 Vytvoření hráčů

Před hrou se program ptá na hráče, dokud uživatel neodpoví `n` (a neexistují alespoň dva):

| Dotaz | Odpověď |
|---|---|
| `Přejete si přidat nového hráče? (a/n)` | `a` (ano) nebo `n` (ne; vyžaduje alespoň 2 hráče) |
| `Zadejte jméno hráče (max 20 znaků):` | jméno o délce 1 až 20 znaků, mezery jsou povoleny, prázdné řádky se přeskočí |
| `Je to člověk nebo AI? (c/a):` | `c` (člověk) nebo `a` (AI) |
| `Obtížnost AI (1 = opatrný, 2 = vyvážený, 3 = riskující):` | `1`, `2` nebo `3`; jiná odpověď zvolí obtížnost 2 |

U jednopísmenných odpovědí se čte první znak (např. `ano` se bere jako `a`).

### 5.2 Ovládání během tahu

| Situace | Co zadat |
|---|---|
| Výzva `Zadej čísla kostek k odložení (1-6, odděleně mezerou):` | čísla bodujících kostek oddělená mezerou, např. `1 5` pro první a pátou kostku. Neplatný výběr se odmítne a program se zeptá znovu. |
| Výzva `Chceš házet znovu (h), nebo uložit skóre (s)?` | `h` házet zbývajícími kostkami, `s` uložit body a předat tah |
| Předčasné ukončení | Ctrl+C, případně konec vstupu (Ctrl+D) |

Počítačoví hráči se rozhodují sami a svůj výběr vypisují.

### 5.3 Ukázka výpisu

Tah člověka (zkrácený výpis ze scénáře `human_invalid_moves`):

```
---Na tahu je TestHrac (Skóre: 0)---

---KOSTKY---
kostka č.1 [1]
kostka č.2 [4]
kostka č.3 [6]
kostka č.4 [1]
kostka č.5 [3]
kostka č.6 [4]
Zadej čísla kostek k odložení (1-6, odděleně mezerou): 1 4
Hráč TestHrac v tomto hodu uhrál 200 bodů, (v tomto kole celkem 200 bodů).
Aktuální body v tahu: 200 | Volných kostek: 4. Chceš házet znovu (h), nebo uložit skóre (s)? s
Hráč TestHrac stále nesplnil vstupní limit, získává tedy 0 bodů.
```

Horké kostky u počítačového hráče (scénář `ai_match`):

```
---KOSTKY---
kostka č.1 [1]
kostka č.2 [1] uzamčená
kostka č.3 [1] uzamčená
kostka č.4 [1] uzamčená
kostka č.5 [5] uzamčená
kostka č.6 [5] uzamčená
Hráč BotRiskujici vybral všechny dostupné kostky.

HORKÉ KOSTKY!!! Všechny kostky bodovaly a může házet znovu se všemi.
Hráč BotRiskujici v tomto hodu uhrál 100 bodů, (v tomto kole celkem 500 bodů).
```

Poslední kolo a konec hry (scénář `ai_match`):

```
Hráč BotRiskujici přesáhl hranici 10000 bodů. Ostatní hráči odehrají poslední kolo.
...
=== KONEC HRY ===

---SKÓRE HRÁČŮ---
BotOpatrny: 5300
BotRiskujici: 10450

Hráč BotRiskujici zvítězil se skórem: 10450.
```

---

## 6. Testování

Testování má tři části: **jednotkové testy** jednotlivých tříd, **scénáře s přesměrovaným vstupem** pro celou hru a **ruční testy** přepínačů a hraničních situací. Díky přepínači `--seed` jsou hody při testech přesně reprodukovatelné. Jednotkové testy používají knihovnu Catch2 [2], která je v zadání povolena. Je oddělená v cíli `farkle_tests`, takže hra (cíl `Farkle`) na ní nezávisí.

### 6.1 Jednotkové testy (`./build/farkle_tests`)

Zdroj: `tests/test_farkle.cpp`, 8 testovacích případů, 111 asercí. Očekávaný výsledek: `All tests passed (111 assertions in 8 test cases)`.

| Testovací případ | Co ověřuje |
|---|---|
| Scorer – základní jednotlivé kostky | `{1}`=100, `{5}`=50, `{1,5}`=150, `{1,1,5,5}`=300 |
| Scorer – trojice a více stejných hodnot | trojice všech hodnot (300, 200, 300, 400, 500, 600), čtveřice, pětice, šestice |
| Scorer – speciální šestikostkové kombinace | postupka, tři páry, čtveřice s dvojicí, dvě trojice |
| Scorer – neplatné kombinace a detekce Farkle | prázdný výběr a výběr s nebodující kostkou (`<= 0`), `hasScoringMove` pro Farkle i pro platné tahy (včetně tří párů a postupky), výjimka `std::invalid_argument` pro hodnotu 0 |
| Dice a Player – validace vstupu a výjimky | `Dice::setValue` mimo 1 až 6 vyhodí výjimku; prázdné jméno i jméno nad 20 znaků vyhodí výjimku |
| Player – skóre a vstup do hry | počáteční stav, `setOnBoard`, `addScore`, `setTotalScore` |
| Dice – zamykání a deterministický hod | zamykání kostky; dva generátory se stejným seedem dávají stejnou posloupnost hodů v rozsahu 1 až 6 |
| AIPlayer – výběr kostek a rozhodování | výběr všech kostek u postupky; ve smíšeném hodu jen bodované kostky; pravidla pro další hod (nutnost vstoupit do hry, ukládání podle `bankThreshold` a `minDiceToReroll`, házení s více než 4 kostkami) |

### 6.2 Scénáře s přesměrovaným vstupem (`tests/inputs/`)

Vstupní soubory obsahují odpovědi uživatele, jeden na řádek. Spouštějí se z kořene projektu. Všechny skončí s kódem 0.

#### Test I-1: standardní průběh hry a výhra – `ai_match.txt`

- **Příkaz:** `./build/Farkle --seed 1 < tests/inputs/ai_match.txt`
- **Vstup:** dva hráči, `BotOpatrny` (AI, obtížnost 1) a `BotRiskujici` (AI, obtížnost 3).
- **Co se testuje:** celá partie od založení hráčů po vyhlášení vítěze. Výstup obsahuje 23 tahů, 5× Farkle, 6× horké kostky, 2× splnění vstupního limitu a poslední kolo.
- **Očekávané chování:** hra se dohraje do konce, vyhlásí vítěze a program skončí s kódem 0.
- **Očekávaný výstup (konec):**

```
=== KONEC HRY ===

---SKÓRE HRÁČŮ---
BotOpatrny: 5300
BotRiskujici: 10450

Hráč BotRiskujici zvítězil se skórem: 10450.
```

#### Test I-2: remíza po posledním kole – `draw.txt`

- **Příkaz:** `./build/Farkle --seed 101 < tests/inputs/draw.txt`
- **Vstup:** dva opatrní počítačoví hráči `Bot1` a `Bot2` (obtížnost 1).
- **Co se testuje:** shodné konečné skóre obou hráčů. Výstup obsahuje 37 tahů, 1× Farkle, 3× horké kostky a poslední kolo.
- **Očekávané chování:** program rozpozná shodné skóre a místo vítěze vypíše remízu.
- **Očekávaný výstup (konec):**

```
=== KONEC HRY ===

---SKÓRE HRÁČŮ---
Bot1: 10250
Bot2: 10250

Hráči: Bot1, Bot2 remízovali se skórem: 10250.
```

- **Jak byl seed nalezen.** Remíza je vzácná (asi 0,5 % partií), proto byl seed hledán. Dočasný cyklus v kódu spouštěl partie dvou opatrných AI se seedy 1 až 5000 s potlačeným výpisem a první remízu našel u seedu 101. Dočasný kód byl po nalezení z programu odstraněn. Totéž lze zopakovat příkazem:

```bash
for s in $(seq 1 5000); do
  if ./build/Farkle --seed $s < tests/inputs/draw.txt | grep -q remízovali; then
    echo "první remíza: seed $s"; break
  fi
done
```

#### Test I-3: neplatné vstupy při zakládání hry a konec vstupu – `edge_setup.txt`

- **Příkaz:** `./build/Farkle --seed 1 < tests/inputs/edge_setup.txt`
- **Co se testuje:** ošetření chybných odpovědí při nastavení hráčů. Pro každý chybný vstup je očekávané chování, že **program se nezacyklí, vypíše chybovou hlášku a znovu se zeptá**.

| Krok | Vstup | Očekávané chování | Očekávaný výstup |
|---|---|---|---|
| 1 | `a` | zahájí přidání hráče | `Zadejte jméno hráče (max 20 znaků):` |
| 2 | prázdný řádek | přeskočí se bez hlášky | výzva čeká na další vstup |
| 3 | `PrilisDlouheJmenoViceNezDvacetZnaku` (35 znaků) | jméno se odmítne, program se zeptá znovu | `Neplatná délka jména, zkuste to znovu.` |
| 4 | `Alice` | jméno se přijme | `Je to člověk nebo AI? (c/a):` |
| 5 | `x` | neplatný typ hráče, program se zeptá znovu | `Neplatná volba, zkus znovu.` |
| 6 | `a` | hráč je AI | `Obtížnost AI (1 = opatrný, 2 = vyvážený, 3 = riskující):` |
| 7 | `9` | neplatná obtížnost, použije se výchozí | `Neplatná volba, automaticky zvolena obtížnost 2 (vyvážený).` |
| 8 | `n` (jen 1 hráč) | hru nelze spustit, program se zeptá znovu | `Hra musí obsahovat alespoň 2 hráče.` |
| 9 | konec souboru (EOF) | program se **nezacyklí**, vypíše hlášku a skončí s kódem 0 | `Konec vstupu, ukončuji program.` |

#### Test I-4: neplatné herní tahy člověka – `human_invalid_moves.txt`

- **Příkaz:** `./build/Farkle --seed 2 < tests/inputs/human_invalid_moves.txt`
- **Vstup:** člověk `TestHrac` a AI `Bot` (obtížnost 2). V prvním tahu mají kostky hodnoty `[1, 4, 6, 1, 3, 4]`.
- **Co se testuje:** ošetření chybného vstupu při výběru kostek a při rozhodování o dalším hodu. U každého neplatného vstupu je očekávané chování, že **hra výběr odmítne, vypíše hlášku a zeptá se znovu**.

| Vstup | Proč je neplatný | Očekávaný výstup |
|---|---|---|
| `0` | číslo kostky mimo 1 až 6 | `Neplatný výběr kostek, zkus to znovu.` |
| `7` | číslo kostky mimo 1 až 6 | `Neplatný výběr kostek, zkus to znovu.` |
| `1 1` | duplicitní kostka (kostka lze bodovat sama) | `Neplatný výběr kostek, zkus to znovu.` |
| prázdný řádek | prázdný výběr | `Neplatný výběr kostek, zkus to znovu.` |
| `abc` | nečíselný vstup | `Neplatný výběr kostek, zkus to znovu.` |
| `1 2 3 4 5 6` | obsahuje nebodující kostky | `Neplatný výběr kostek, zkus to znovu.` |
| `1,4` | čárka mezi čísly (nečíselný znak, výběr se neinterpretuje částečně) | `Neplatný výběr kostek, zkus to znovu.` |
| `1 4` | **platný výběr** dvou jedniček | `Hráč TestHrac v tomto hodu uhrál 200 bodů, (v tomto kole celkem 200 bodů).` |
| `x` (na dotaz `h`/`s`) | neplatná volba akce | `Neplatná volba, zadej 'h' nebo 's'.` |
| `s` | **platné** uložení 200 bodů pod vstupním limitem 500 | `Hráč TestHrac stále nesplnil vstupní limit, získává tedy 0 bodů.` |
| konec souboru v dalším kole | konec vstupu při výzvě k výběru kostek | `Konec vstupu, ukončuji program.`, kód 0 |

Scénář zároveň ukazuje **Farkle** u počítačového hráče: `Bot` odloží dvě jedničky a v dalším hodu nehodí žádnou bodovanou kostku (`FARKLE! Hráč Bot spálil tah (přichází o všechny body z kola).`).

### 6.3 Ruční testy přepínačů a hraničních situací

| Situace | Příkaz | Očekávaný výstup / chování |
|---|---|---|
| Nápověda | `./build/Farkle --help` | vypíše `=== MOŽNOSTI ===`, ovládání a pravidla, kód 0 |
| Chybějící hodnota seedu | `./build/Farkle --seed` | `Chyba: Přepínač --seed vyžaduje číselnou hodnotu.`, kód 1 |
| Neplatný seed | `./build/Farkle --seed abc` | `Chyba: Neplatná hodnota pro --seed (zadejte celé číslo).`, kód 1 |
| Prázdný vstup od začátku | `./build/Farkle < /dev/null` | `Konec vstupu, ukončuji program.`, kód 0, bez zacyklení |
| Jméno o délce 20 znaků | zadat 20× `A` jako jméno | jméno se přijme |
| Jméno o délce 21 znaků | zadat 21× `A` jako jméno | `Neplatná délka jména, zkuste to znovu.` |
| Tři hráči a poslední kolo | `printf 'a\nA\na\n1\na\nB\na\n2\na\nC\na\n3\nn\n' \| ./build/Farkle --seed 2` | hráč B překročí 10 000, poté C a A odehrají po jednom tahu; skóre A 6700, B 10500, C 3900; vítězí B s 10 500 body |

### 6.4 Přehled herních scénářů

| Scénář | Kde je pokrytý |
|---|---|
| **Výhra** | I-1 (vyhrává `BotRiskujici`), ruční test tří hráčů (vyhrává B) |
| **Prohra** | I-1 (`BotOpatrny` prohrává se 5 300 body) |
| **Remíza** | I-2 (10 250 : 10 250) |
| **Farkle** | I-1, I-2 a I-4 |
| **Horké kostky** | I-1 (6×) a I-2 (3×) |
| **Splnění vstupního limitu** | I-1 a I-2 (každý 2×) |
| **Nesplnění vstupního limitu** | I-4 |
| **Poslední kolo** | I-1, I-2, ruční test tří hráčů |
| **Neočekávaný vstup od uživatele** | I-3 (nastavení hráčů), I-4 (výběr kostek a akce), ruční testy přepínačů |
| **Konec vstupu (EOF)** | I-3, I-4, prázdný vstup |
| **Vypršení časového limitu** | **není relevantní**: hra žádný časový limit nemá |

Díky polymorfismu (`Player*`) je vyhodnocení konce hry pro člověka i počítač stejný kód, takže výhra, prohra i remíza se chovají stejně bez ohledu na typ hráče.

---

## 7. Videa a časový harmonogram

| Video | Obsah | Odkaz |
|---|---|---|
| 1 – překlad programu | příkazy pro překlad, spuštění překladu a celý jeho průběh | _doplnit odkaz_ |
| 2 – průběh hry a testování | hra, výhra, prohra, hraniční situace a komentář k testování | _doplnit odkaz_ |

### Časový harmonogram videa 1

| Čas | Scénář |
|---|---|
| | |

### Časový harmonogram videa 2

| Čas | Scénář |
|---|---|
| | |

---

## 8. Použité zdroje a využití AI

### 8.1 Zdroje

1. Zadání semestrální práce. BRUTE, ČVUT FEL, 2026.
2. **Catch2** v2.13.10 – testovací framework pro C++. <https://github.com/catchorg/Catch2/releases/tag/v2.13.10>
3. **YouTube** Návod ke hře Farkle pro pochopení průběhu hry a pravidel. <https://www.youtube.com/watch?v=z5hVT2Alt-0>
4. **Gemini** (Google), model **Gemini Hydrogen 4**, <https://gemini.google.com> – AI asistent, viz kapitola 8.2.
5. **Claude** (Anthropic), model **Claude Sonnet 5.5**, <https://claude.ai> – AI asistent, viz kapitola 8.2.

Pravidla hry Farkle jsou veřejně známá a existuje více jejich variant; konkrétní bodování použité v programu je uvedeno v kapitole 1.1.

### 8.2 Využití AI nástrojů

Práce vznikla ve spolupráci s AI asistenty, kteří sloužili jako konzultanti a oponenti, **nikoli jako generátor celého projektu**. Asistenti navrhovali kostru tříd (obsažené metody a atributy), kterou následně autor posoudil, upravil dle potřeb a jednotlivé metody sám implementoval. Implementované metody byly asistentmi zkontrolovány pro neověřené vstupy, korektní funkčnost a možné efektivnější řešení. Autor odezvu asistentů vzal v potaz a případné chyby opravil nebo řešení přeimplementoval efektivněji.

| Nástroj | K čemu byl použit |
|---|---|
| **Gemini** (Google), model Gemini Hydrogen 4 | **Návrh architektury:** konzultace rozdělení odpovědností mezi třídy (`Scorer` jako bezstavový vyhodnocovač, `Game` jako správce herní smyčky) a využití polymorfismu přes `std::unique_ptr<Player>`. **Okrajové případy vstupu:** návrh robustního parsování řádku v `HumanPlayer::chooseDice` pomocí `std::stringstream` (včetně `ss.clear(); ss >> std::ws; if (!ss.eof()) return {};`) a bezpečného ukončení smyček při konci vstupu. **Testovací strategie:** struktura jednotkových testů v Catch2, příprava scénářů s přesměrovaným vstupem a jednorázový vyhledávací cyklus pro nalezení seedu remízy (kapitola 6.2). **Přenositelnost:** úprava `CMakeLists.txt` (`cmake_minimum_required` 3.10, C++14 bez rozšíření kompilátoru) a sjednocení názvů binárek. |
| **Claude** (Anthropic), model Claude Sonnet 5.5 | **Revize kódu a testů:** nalezení chybějícího `#include <array>` v `Scorer.hpp`, zacyklení při konci vstupu, chyba v kódu `if (!std::cin)` v `main.cpp`, tichého částečného přijetí vstupu typu `1,4`, přenositelnosti seedu (náhrada `std::uniform_int_distribution` za `rng() % 6`) a příliš přísného `cmake_minimum_required`. **Simulace AI:** změření a vyvážení tří úrovní obtížnosti (kapitola 1.3). **Dokumentace:** pomoc při tvorbě tohoto dokumentu (syntaxe, struktura, tabulky, diagram tříd) a ověření všech v něm uvedených příkazů a výstupů spuštěním programu. |

Tabulku v podkapitole 8.2 si každý model vytvořil samostatně, aby se předešlo neúmyslnému vynechání částí vypracovaných asistenty. Autor odpovídá za veškerý obsah práce a je schopen jej vysvětlit.