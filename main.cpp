#include <iostream>
#include "earley_algorithm.h"

using namespace grammar;

signed main() {
    StdReader stdReader;
    auto grammar = stdReader.GetGrammar();
    auto words = stdReader.GetWords();
    EarleySolver earleySolver(grammar);
    for (const auto& word : words) {
        std::cout << (earleySolver.PredictWord(word) ? "Yes" : "No") << "\n";
    }
    return 0;
}