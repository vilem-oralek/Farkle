cmake_minimum_required(VERSION 3.10)

project(Farkle)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(farkle_tests
        tests/catch.cpp
        tests/test_farkle.cpp
        src/Dice.cpp
        src/Scorer.cpp
        src/Player.cpp
        src/HumanPlayer.cpp
        src/AIPlayer.cpp
)
