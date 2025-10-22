#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <random>

#include "Word.h"

std::vector<std::string> getWordList(const std::string& filename, const int length) {
    auto words = std::vector<std::string>();
    std::ifstream file(filename);

    std::string line;
    while (std::getline(file, line)) {
        if (line.length() == length) {
            words.push_back(line);
        }
    }

    return words;
}

int getDuplicates(const std::string& word) {
    auto letterCount = std::map<char, int>();
    int count = 0;
    for (const auto letter : word) {
        if (++letterCount[letter] > 1) {
            count++;
        }
    }
    return count;
}

int getRanking(const std::map<char, int>& letterCount, const std::string& word) {
    int ranking = 0;

    for (const auto letter : word) {
        if (letterCount.contains(letter)) {
            ranking += letterCount.at(letter);
        }
    }

    return ranking;
}

std::map<char, int> getLetterCount(const std::vector<std::string>& wordsList) {
    auto letterCount = std::map<char, int >();

    for (const auto& word : wordsList) {
        for (const auto letter : word) {
            letterCount[letter]++;
        }
    }

    return letterCount;
}

std::vector<Word> getWords(const std::vector<std::string>& wordsList) {
    auto words = std::vector<Word>();
    const auto letterCount = getLetterCount(wordsList);

    for (const auto& word : wordsList) {
        const int duplicates = getDuplicates(word);
        const int ranking = getRanking(letterCount, word);
        words.emplace_back(word, duplicates, ranking);
    }

    return words;
}

Word getRandomWord(const std::vector<Word>& words) {
    constexpr int min = 0;
    const auto max = words.size() - 1;

    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

    const auto index = dist(rng);

    return words[index];
}

std::set<char> getGuessedLetters(const std::vector<Word>& guessedWords) {
    auto guessedLetters = std::set<char>();
    for (const auto& word : guessedWords) {
        for (const char letter : word.getWord()) {
            guessedLetters.insert(letter);
        }
    }
    return guessedLetters;
}

std::set<char> getExcludedLetters(const std::set<char>& guessedLetters, const std::set<char>& knownLetters) {
    auto excludedLetters = std::set<char>();

    for (const auto& letter : guessedLetters) {
        if (!knownLetters.contains(letter)) {
            excludedLetters.insert(letter);
        }
    }

    return excludedLetters;
}

bool isWordPossible(
    const Word& word,
    const std::set<char>& guessedLetters,
    const std::set<char>& knownLetters,
    const std::map<int, char>& knownPositions,
    const std::map<int, std::set<char>>& excludedPositions) {
    const auto& excludedLetters = getExcludedLetters(guessedLetters, knownLetters);
    // The word is not possible if the known positions aren't present in the word
    for (const auto& [index, letter] : knownPositions) {
        if (word.getWord().length() <= index || word.getWord()[index] != letter) {
            return false;
        }
    }

    // The word is not possible if a letter is excluded from the position
    for (int i = 0; i < word.getWord().length(); i++) {
        if (excludedPositions.contains(i) && excludedPositions.at(i).contains(word.getWord().at(i))) {
            return false;
        }
    }

    // The word is not possible if the word doesn't contain the known letters
    for (const auto letter : knownLetters) {
        if (!word.getWord().contains(letter)) {
            return false;
        }
    }

    // The word is not possible if the word does contain excluded letters
    if (!std::ranges::all_of(excludedLetters, [&](const char letter) {
        return !word.getWord().contains(letter);
    })) {
        return false;
    }

    return true;
}

std::vector<Word> getPossibleWords(
    const std::vector<Word>& possibleWords,
    const std::set<char>& guessedLetters,
    const std::set<char>& knownLetters,
    const std::map<int, char>& knownPositions,
    const std::map<int, std::set<char>>& excludedPositions) {
    auto newPossibleWords = std::vector<Word>();

    for (const auto& word : possibleWords) {
        if (isWordPossible(word, guessedLetters, knownLetters, knownPositions, excludedPositions)) {
            newPossibleWords.push_back(word);
        }
    }

    return newPossibleWords;
}

std::vector<Word> getWordsWithoutDuplicates(const std::vector<Word>& words, const int allowedDuplicates) {
    std::vector<Word> result;
    result.reserve(words.size());

    for (const auto& word : words) {
        if (word.getDuplicates() <= allowedDuplicates) {
            result.push_back(word);
        }
    }

    return result;
}

Word getMostRelevantWord(const std::vector<Word>& possibleWords) {
    if (possibleWords.empty()) {
        return {{}, 0, 0};
    }

    int allowedDuplicates = 0;
    std::vector<Word> words;
    do {
        words = getWordsWithoutDuplicates(possibleWords, allowedDuplicates);
        allowedDuplicates++;
    } while (words.empty());

    std::sort(words.begin(), words.end());

    return words.at(0);
}

void updateKnownLetters(
    std::set<char>& knownLetters,
    const Word& currentGuess,
    const std::string& targetWord) {
    for (const auto letter : targetWord) {
        if (currentGuess.getWord().contains(letter) && !knownLetters.contains(letter)) {
            knownLetters.insert(letter);
        }
    }
}

void updateKnownPositions(
    std::map<int, char>& knownPositions,
    const Word& currentGuess,
    const std::string& targetWord) {
    if (currentGuess.getWord().length() != targetWord.length()) {
        return;
    }

    for (int i = 0; i < targetWord.length(); i++) {
        if (targetWord[i] == currentGuess.getWord()[i] && !knownPositions.contains(i)) {
            knownPositions.insert({i, targetWord[i]});
        }
    }
}

void updateExcludedPositions(
    std::map<int, std::set<char>>& excludedPositions,
    const Word& currentGuess,
    const std::string& targetWord) {
    if (currentGuess.getWord().length() != targetWord.length()) {
        return;
    }

    for (int i = 0; i < targetWord.length(); i++) {
        if (targetWord[i] != currentGuess.getWord()[i]) {
            if (excludedPositions.contains(i)) {
                excludedPositions.at(i).insert(currentGuess.getWord()[i]);
            } else {
                excludedPositions.insert({i, {currentGuess.getWord()[i]}});
            }
        }
    }
}

int main() {
    const std::string filename = "../res/words_alpha.txt";
    constexpr int wordLength = 5;

    const auto words = getWordList(filename, wordLength);
    if (words.empty()) {
        std::cout << "No words found with length " << wordLength << std::endl;
        return 0;
    }

    auto possibleWords = getWords(words);

    const auto targetWord = getRandomWord(possibleWords).getWord();
    std::cout << "Target word: '" << targetWord << "'" << std::endl;

    auto guessedWords = std::vector<Word>();
    auto knownLetters = std::set<char>();
    auto knownPositions = std::map<int, char>();
    auto excludedPositions = std::map<int, std::set<char>>();
    Word currentGuess;
    int guesses = 0;

    do {
        currentGuess = getMostRelevantWord(possibleWords);
        guesses++;

        if (currentGuess.getWord().empty()) {
            std::cout << "Could not find the word because there are no possible words left." << std::endl;
            break;
        }
        std::cout << "Guess " << guesses << ":     '" << currentGuess.getWord() << "'" << std::endl;

        guessedWords.push_back(currentGuess);
        const auto& guessedLetters = getGuessedLetters(guessedWords);
        updateKnownLetters(knownLetters, currentGuess, targetWord);
        updateKnownPositions(knownPositions, currentGuess, targetWord);
        updateExcludedPositions(excludedPositions, currentGuess, targetWord);
        possibleWords = getPossibleWords(possibleWords, guessedLetters, knownLetters, knownPositions, excludedPositions);
    } while (targetWord != currentGuess.getWord());

#ifdef NDEBUG
    std::cout << std::endl << "Press any key to continue..." << std::endl;
    getchar();
#endif

    return 0;
}
