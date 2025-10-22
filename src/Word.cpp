//
// Created by Leon on 22.10.2025.
//

#include "Word.h"

#include <utility>

Word::Word() : Word("", 0, 0) {

}

Word::Word(std::string word, const int duplicates, const int ranking) : word(std::move(word)), duplicates(duplicates), ranking(ranking) {

}

bool Word::operator<(const Word &other) const {
    return other.ranking < ranking;
}
