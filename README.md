# Farkle


--seed 1 : ai_match.txt
    BotOpatrny: 5300
    BotRiskujici: 10450

--seed 101 : draw.txt
    Bot1: 10250
    Bot1: 10250

--seed 1 : edge_setup.txt
    white space - ignorováno
    dlouhé jméno - Neplatná délka jména, zkuste to znovu.
    neplatny typ AI - Neplatná volba, zkus znovu.
    neplatná obtížnost AI - Neplatná volba, automaticky zvolena obtížnost 2 (vyvážený).
    pouze 1 hráč - Hra musí obsahovat alespoň 2 hráče.
    konec vstupního souboru (EOF) - Konec vstupu, ukončuji program.

--seed 2 : human_invalid_moves
    výběr kostky 0 - Neplatný výběr kostek, zkus to znovu.
    výběr kostky 7 - Neplatný výběr kostek, zkus to znovu.
    duplicitní výběr kostky která boduje sama - Neplatný výběr kostek, zkus to znovu.
    white space - Neplatný výběr kostek, zkus to znovu.
    abc - Neplatný výběr kostek, zkus to znovu.
    výběr obsahující nebodující kostky - Neplatný výběr kostek, zkus to znovu.
    čárka mezi bodujícími kostkami - Neplatný výběr kostek, zkus to znovu.
    výběr dvou kostek s hodnotami 1 - Hráč TestHrac v tomto hodu uhrál 200 bodů, (v tomto kole celkem 200 bodů).
    odpověď "x" na "Chceš házet znovu (h), nebo uložit skóre (s)?" - Neplatná volba, zadej 'h' nebo 's'.
    uložení skóre menší jak 500 - Hráč TestHrac stále nesplnil vstupní limit, získává tedy 0 bodů.
    konec vstupního souboru (EOF) v dalším kole při výzvě k výběru kostek – Konec vstupu, ukončuji program.