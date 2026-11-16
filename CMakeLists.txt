cmake_minimum_required(VERSION 4.3)
project(Farkle)

set(CMAKE_CXX_STANDARD 14)

add_executable(Farkle main.cpp
        src/Dice.cpp
        src/Dice.hpp
        src/Scorer.cpp
        src/Scorer.hpp
        src/Player.cpp
        src/Player.hpp
        src/HumanPlayer.cpp
        src/HumanPlayer.hpp
        src/AIPlayer.cpp
        src/AIPlayer.hpp
        src/Game.cpp
        src/Game.hpp
)
