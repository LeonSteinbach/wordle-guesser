//
// Created by Leon on 22.10.2025.
//

#ifndef WORDLE_WORD_H
#define WORDLE_WORD_H

#include <string>

class Word {
    std::string word;
    int duplicates{};
    int ranking{};

public:
    Word();
    Word(std::string word, int duplicates, int ranking);

    bool operator < (const Word& other) const;

    [[nodiscard]] std::string getWord() const { return word; }
    [[nodiscard]] int getDuplicates() const { return duplicates; }
    [[nodiscard]] int getRanking() const { return ranking; }
};

#endif //WORDLE_WORD_H