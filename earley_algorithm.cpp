#include "earley_algorithm.h"

grammar::Rule::Rule(char non_terminal, const std::string& product) :
    non_terminal(non_terminal), product(product) {}

size_t grammar::Rule::GetProductSize() const {
    return product.size();
}

char grammar::Rule::GetNonTerminal() const {
    return non_terminal;
}
const std::string& grammar::Rule::GetProduct() const {
    return product;
}
std::string& grammar::Rule::GetProduct() {
    return product;
}
bool grammar::Rule::operator==(const Rule& other) const {
    return non_terminal == other.GetNonTerminal() && product == other.GetProduct();
}


grammar::Situation::Situation(const Rule& rule, size_t layer_ind, size_t divider_ind) :
    rule(rule), layer_ind(layer_ind), divider_ind(divider_ind) {}
const grammar::Rule& grammar::Situation::GetRule() const {
    return rule;
}
grammar::Rule& grammar::Situation::GetRule() {
    return rule;
}
size_t grammar::Situation::GetLayerInd() const {
    return layer_ind;
}
size_t grammar::Situation::GetDividerInd() const {
    return divider_ind;
}

bool grammar::Situation::operator==(const Situation& other) const {
    return rule == other.rule && layer_ind == other.layer_ind && divider_ind == other.divider_ind;
}


size_t std::hash<grammar::Rule>::operator()(const grammar::Rule& rule) const {
    return std::hash<std::string>()(
        std::string{rule.GetNonTerminal()} + "->" + rule.GetProduct()
    );
}

size_t std::hash<grammar::Situation>::operator()(const grammar::Situation& situation) const {
    return std::hash<std::string>()(
        std::string(
            std::string{situation.GetRule().GetNonTerminal()} + "->" +
            situation.GetRule().GetProduct() +
            "div:" + to_string(situation.GetDividerInd()) +
            "lay:" + to_string(situation.GetLayerInd()))
    );
}


char grammar::Grammar::GetFAKESTART() {
    return FAKESTART;
}

char grammar::Grammar::GetStartingNonTerminal() const {
    return starting_non_terminal;
}
std::unordered_set<char>& grammar::Grammar::GetNonTerminals() {
    return non_terminals;
}

const std::unordered_set<char>& grammar::Grammar::GetNonTerminals() const {
    return non_terminals;
}

std::unordered_set<char>& grammar::Grammar::GetTerminals() {
    return terminals;
}

const std::unordered_set<char>& grammar::Grammar::GetTerminals() const {
    return terminals;
}

std::unordered_set<grammar::Rule>& grammar::Grammar::GetRules() {
    return rules;
}

const std::unordered_set<grammar::Rule>& grammar::Grammar::GetRules() const {
    return rules;
}

grammar::Grammar::Grammar(char starting_non_terminal, const std::unordered_set<char>& non_terminals,
                          const std::unordered_set<char>& terminals, const std::unordered_set<Rule> rules) :
    starting_non_terminal(starting_non_terminal),
    non_terminals(non_terminals), terminals(terminals), rules(rules) {}
grammar::Grammar::Grammar() = default;

grammar::Grammar grammar::StdReader::GetGrammar() const {
    std::unordered_set<char> non_terminals;
    std::unordered_set<char> terminals;
    std::unordered_set<Rule> rules;
    char starting_non_terminal;

    size_t non_terminals_num, terminals_num, rules_num;

    std::cin >> non_terminals_num >> terminals_num >> rules_num;
    for (size_t i = 0; i < non_terminals_num; ++i) {
        char cur_non_terminal;
        std::cin >> cur_non_terminal;
        if (!IsNonTerminal(cur_non_terminal)) {
            throw std::invalid_argument("invalid non terminal, found " + std::string{cur_non_terminal});
        }
        non_terminals.emplace(cur_non_terminal);
    }
    for (size_t i = 0; i < terminals_num; ++i) {
        char cur_terminal;
        std::cin >> cur_terminal;
        if (!IsTerminal(cur_terminal)) {
            throw std::invalid_argument("invalid terminal, found " + std::string{cur_terminal});
        }
        terminals.emplace(cur_terminal);
    }
    for (size_t i = 0; i < rules_num; ++i) {
        std::string cur_rule;
        if (i == 0) {
            getline(std::cin, cur_rule);
        }
        getline(std::cin, cur_rule);

        if (cur_rule.size() < 5) {
            throw std::invalid_argument("rule size less than 5, it's impossible, found " + cur_rule);
        }
        if (cur_rule[1] != ' ' || cur_rule[2] != '-' || cur_rule[3] != '>' || cur_rule[4] != ' ') {
            throw std::invalid_argument(
                "rule format is strange, did not found *space*->*space* construction, found " + cur_rule);
        }
        if (!IsNonTerminal(cur_rule[0])) {
            throw std::invalid_argument("rule first element isn't a non-terminal, found " + cur_rule);
        }
        char non_terminal = cur_rule[0];
        if (!non_terminals.contains(non_terminal)) {
            throw std::invalid_argument(
                "we don't have non_terminal like this! found " + std::string{cur_rule[0]});
        }
        for (size_t rule_iter = 5; rule_iter < cur_rule.size(); ++rule_iter) {
            if (!non_terminals.contains(cur_rule[rule_iter]) && !terminals.contains(cur_rule[rule_iter])) {
            }

            if (!non_terminals.contains(cur_rule[rule_iter]) && !terminals.contains(cur_rule[rule_iter])) {
                throw std::invalid_argument("we don't have this symbol from target! found (rule + symbol)" +
                                            cur_rule + " + " + std::string{cur_rule[rule_iter]});
            }
        }
        rules.emplace(non_terminal, cur_rule.substr(5, cur_rule.size() - 5));

    }
    std::cin >> starting_non_terminal;
    if (!non_terminals.contains(starting_non_terminal)) {
        throw std::invalid_argument(
            "starting non terminal is invalid, there is no non terminal like this, found: " +
            std::string{starting_non_terminal});
    }
    grammar::Grammar result{starting_non_terminal, non_terminals, terminals, rules};
    return result;
}
std::vector<std::string> grammar::StdReader::GetWords() const {
    size_t num_words;
    std::cin >> num_words;
    std::vector<std::string> to_ret(num_words);
    for (auto& word: to_ret) {
        std::cin >> word;
    }
    return to_ret;
}

void grammar::EarleySolver::Promotion(size_t layer_to_fill) {
    if (layer_to_fill == 0) {
        layers[0].emplace(Rule(Grammar::GetFAKESTART(),
                               std::string{grammar.GetStartingNonTerminal()}), 0, 0);

        return;
    }

    std::vector<Situation> new_situations;

    for (const auto& situation: layers[layer_to_fill - 1]) {
        if (situation.GetRule().GetProductSize() > situation.GetDividerInd() &&
            situation.GetRule().GetProduct()[situation.GetDividerInd()] == word[layer_to_fill - 1]) {
            new_situations.emplace_back(Situation(situation.GetRule(),
                                                  situation.GetLayerInd(),
                                                  situation.GetDividerInd() + 1));
        }
    }

    for (const auto& new_situation: new_situations) {
        layers[layer_to_fill].emplace(new_situation);
    }

}

void grammar::EarleySolver::Disclosure(size_t layer_to_fill) {
    std::vector<Situation> new_situations;

    for (const auto& situation: layers[layer_to_fill]) {
        if (situation.GetRule().GetProductSize() > situation.GetDividerInd()) {
            for (const auto& rule: grammar.GetRules()) {
                if (rule.GetNonTerminal() ==
                    situation.GetRule().GetProduct()[situation.GetDividerInd()]) {
                    new_situations.emplace_back(Situation(rule, layer_to_fill, 0));
                }
            }
        }
    }

    for (const auto& new_situation: new_situations) {
        layers[layer_to_fill].emplace(new_situation);
    }
}

void grammar::EarleySolver::Completion(size_t layer_to_fill) {
    std::vector<Situation> new_situations;

    for (const auto& situation: layers[layer_to_fill]) {
        if (situation.GetDividerInd() == situation.GetRule().GetProductSize()) {
            for (const auto& other_situation: layers[situation.GetLayerInd()]) {
                if (other_situation.GetRule().GetProductSize() > other_situation.GetDividerInd() &&
                    situation.GetRule().GetNonTerminal() ==
                    other_situation.GetRule().GetProduct()[other_situation.GetDividerInd()]) {
                    new_situations.emplace_back(other_situation.GetRule(),
                                                other_situation.GetLayerInd(),
                                                other_situation.GetDividerInd() + 1);
                }
            }
        }
    }

    for (const auto& new_situation: new_situations) {
        layers[layer_to_fill].emplace(new_situation);
    }
}

bool grammar::EarleySolver::PredictWord(const std::string& to_predict) {
    word = to_predict;
    layers.clear();
    layers.resize(to_predict.size() + 1);

    for (size_t layer_ind = 0; layer_ind < word.size() + 1; ++layer_ind) {
        Promotion(layer_ind);

        size_t size_before_add = layers[layer_ind].size();
        size_t size_after_add = size_before_add + 1; // must be different from size_before_add before add
        while (size_before_add != size_after_add) {
            size_before_add = layers[layer_ind].size();
            Disclosure(layer_ind);
            Completion(layer_ind);
            size_after_add = layers[layer_ind].size();
        }
    }
    for (int i = 0; i < layers.size(); ++i) {
        for (auto& kek: layers[i]) {

        }
    }

    return layers.back().contains(Situation{
        Rule(Grammar::GetFAKESTART(), std::string{grammar.GetStartingNonTerminal()}),
        0, 1});
}