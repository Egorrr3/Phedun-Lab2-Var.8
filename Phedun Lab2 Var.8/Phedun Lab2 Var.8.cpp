#include <iostream>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <locale>
#include <vector>
#include <algorithm>
#include "windows.h";

using namespace std;

int maxLength = 10;

class FiniteAutomaton {
public:
    FiniteAutomaton() {}

    void ReadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Can't open file: " << filename << endl;
            return;
        }

        string line;
        ReadAlphabet(file);
        ReadStates(file);
        ReadInitialState(file);
        ReadFinalStates(file);
        while (getline(file, line)) {
            ReadTransition(line);
        }

        file.close();
    }

    bool ProcessInput(const string& input) { // checks if string can be processed by automaton
        currentState = initialState;

        for (char symbol : input) {
            if (transitions_.count(make_pair(currentState, symbol)) == 0) {
                return false;
            }
            currentState = transitions_[make_pair(currentState, symbol)];
        }

        return finalStates.count(currentState) > 0;
    }

    set<char> alphabet;
    set<int> states;
    int initialState;
    set<int> finalStates;
    int currentState;
    map<pair<int, char>, int> transitions_;

    void ReadAlphabet(ifstream& file) {
        string line;
        getline(file, line);
        stringstream ss(line);
        char symbol;
        while (ss >> symbol) {
            if (symbol != ' ') {
                alphabet.insert(symbol);
            }
        }
    }

    void ReadStates(ifstream& file) {
        string line;
        getline(file, line);
        stringstream ss(line);
        int state;
        while (ss >> state) {
            states.insert(state);
        }
    }

    void ReadInitialState(ifstream& file) {
        file >> initialState;
    }

    void ReadFinalStates(ifstream& file) {
        int numFinalStates;
        file >> numFinalStates;
        for (int i = 0; i < numFinalStates; ++i) {
            int finalState;
            file >> finalState;
            finalStates.insert(finalState);
        }
        string line;
        getline(file, line);
    }

    void ReadTransition(const string& line) {
        int fromState, toState;
        char inputSymbol;
        stringstream ss(line);
        ss >> fromState >> inputSymbol >> toState;
        transitions_[make_pair(fromState, inputSymbol)] = toState;
    }
};

void FindWordsWithoutPeriodicFragments(FiniteAutomaton& automaton, string currentWord, int currentState, set<string>& validWords) {
    if (automaton.finalStates.count(currentState) > 0) {
        validWords.insert(currentWord);
    }

    if (currentWord.length() > maxLength) {
        return;
    }

    for (char symbol : automaton.alphabet) {
        int nextState = automaton.transitions_[make_pair(currentState, symbol)];

        FindWordsWithoutPeriodicFragments(automaton, currentWord + symbol, nextState, validWords);
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_CTYPE, "ukr");

    FiniteAutomaton automaton;

    automaton.ReadFromFile("input.txt");

    set<string> validWords;

    try {
        FindWordsWithoutPeriodicFragments(automaton, "", automaton.initialState, validWords);
        vector<string> sortedWords(validWords.begin(), validWords.end());
        sort(sortedWords.begin(), sortedWords.end());
        sortedWords.erase(remove(sortedWords.begin(), sortedWords.end(), ""), sortedWords.end());
        vector<string> filteredWords;
        for (const string& word : sortedWords) {
            bool isContained = false;
            for (const string& smallerWord : sortedWords) {
                if (word == smallerWord) {
                    continue;
                }
                if (word.find(smallerWord) != string::npos) {
                    isContained = true;
                    break;
                }
            }
            if (!isContained) {
                filteredWords.push_back(word);
            }
        }
        cout << "Words without periodic fragments:" << endl;
        for (const string& word : filteredWords) {
            cout << word << endl;
        }
    }
    catch (const exception& ex) {
        cerr << "An error occurred: " << ex.what() << endl;
    }

    system("pause");
    return 0;
}
