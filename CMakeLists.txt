cmake_minimum_required(VERSION 4.0)
project(wordle)

set(CMAKE_CXX_STANDARD 26)

FILE(COPY res/words_alpha.txt DESTINATION "${CMAKE_BINARY_DIR}/res")

add_executable(wordle src/main.cpp
        src/Word.h
        src/Word.cpp)
