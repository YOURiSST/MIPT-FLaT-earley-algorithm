#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace grammar {
    class Rule {
    public:
        Rule(char non_terminal, const std::string& product);

        size_t GetProductSize() const;
        char GetNonTerminal() const;
        const std::string& GetProduct() const;
        std::string& GetProduct();
        bool operator==(const Rule& other) const;

    private:
        char non_terminal;
        std::string product;
    };

    class Situation {
    public:
        Situation(const Rule& rule, size_t layer_ind, size_t divider_ind);
        const Rule& GetRule() const;
        Rule& GetRule();
        size_t GetLayerInd() const;
        size_t GetDividerInd() const;

        bool operator==(const Situation& other) const;

    private:
        Rule rule;
        size_t layer_ind;
        size_t divider_ind;
    };

}  // namespace grammar

namespace std {
    template<>
    struct std::hash<grammar::Rule> {
        size_t operator()(const grammar::Rule& rule) const;
    };

    template<>
    struct std::hash<grammar::Situation> {
        size_t operator()(const grammar::Situation& situation) const;
    };
}

namespace grammar {
    class Grammar {
    public:
        static char GetFAKESTART();

        char GetStartingNonTerminal() const;
        std::unordered_set<char>& GetNonTerminals();

        const std::unordered_set<char>& GetNonTerminals() const;

        std::unordered_set<char>& GetTerminals();

        const std::unordered_set<char>& GetTerminals() const;

        std::unordered_set<Rule>& GetRules();

        const std::unordered_set<Rule>& GetRules() const;

        Grammar(char starting_non_terminal, const std::unordered_set<char>& non_terminals,
                const std::unordered_set<char>& terminals, const std::unordered_set<Rule> rules);
        Grammar();


    private:
        static constexpr char FAKESTART = '_';
        char starting_non_terminal;
        std::unordered_set<char> non_terminals;
        std::unordered_set<char> terminals;
        std::unordered_set<Rule> rules;
    };

    class Reader {
    public:
        virtual Grammar GetGrammar() const = 0;
        virtual std::vector<std::string> GetWords() const = 0;
    protected:
        bool IsNonTerminal(char to_check) const {
            return to_check >= 'A' && to_check <= 'Z';
        }
        bool IsTerminal(char to_check) const {
            return (to_check >= 'a' && to_check <= 'z') ||
                   (to_check >= '0' && to_check <= '9') ||
                   (to_check == '+' || to_check == '-' || to_check == '*' || to_check == '/') ||
                   (to_check == '(' || to_check == ')' || to_check == '[' || to_check == ']' ||
                    to_check == '{' || to_check == '}');
        }
    };

    class StdReader : public Reader {
    public:
        virtual Grammar GetGrammar() const override final;
        virtual std::vector<std::string> GetWords() const override final;
    };

    class EarleySolver {
    public:
        EarleySolver(const Grammar& grammar) : grammar(grammar) {}

        void Promotion(size_t layer_to_fill);

        void Disclosure(size_t layer_to_fill);

        void Completion(size_t layer_to_fill);

        bool PredictWord(const std::string& to_predict);

    private:
        Grammar grammar;
        std::vector<std::unordered_set<Situation>> layers;
        std::string word;
    };
} // namespace grammar



