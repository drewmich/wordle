//
//  main.cpp
//  wordle_solver
//
//  Created by Drew Michelini on 2/9/22.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "xcode_redirect.hpp"
#include "getopt.h"

using namespace std;

class Wordle{
public:
    struct Tile{
        
        Tile(string str, uint32_t o){
            word = str;
            order = 0;
        }
        
        string word;
        uint32_t order;
    };
    
    struct Comp_s{
        bool operator()(Tile* t1, Tile* t2) const{
            
            return t1->word < t2->word;
            
        }
    };
    
    struct Comp_o{
        bool operator()(Tile* t1, Tile* t2){
            
            return t1->order < t2->order;
            
        }
    };
    
    
    vector<Tile*> data;
    set<Tile*, Comp_s> dictionary;
    set<Tile*, Comp_s> working_set;
    set<Tile*, Comp_s> possibleGuesses;
    unordered_map<char, unordered_map<int, set<Tile*, Comp_s> > > map;
    // set<char> remainingLetters = {
    //     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
    //     'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
    //     'u', 'v', 'w', 'x', 'y', 'z'
    //     };

    set<char> guessedLetters;
    
    // int possible_words = -1;
    
    void get_options(int argc, char** argv) {
        int option_index = 0, option = 0;
        
        opterr = true;

        // Use getopt to find command line options
        struct option longOpts[] = {
            { "help", no_argument, nullptr, 'h' },
            { "new", no_argument, nullptr, 'n' },
            { nullptr, 0, nullptr, '\0' }
            };
        
        while ((option = getopt_long(argc, argv, "hn", longOpts,
                                     &option_index)) != -1) {
            switch (option) {
                
                // Beginning word
                case 'h':
                    cout << "A CLI to assist in solving daily wordle challenges!\n";
                    cout << "Example:" << endl;
                    cout << "Enter indices of green tiles, seperated by spaces, end with -1: " << endl;
                    cout << "0 2 3 -1" << endl;
                    break;
                
                case 'n':
                    cout << "Starting up. . . \n";
                    break;
                    
            }
        }
        
        
    } // get_options
    
    void readInDict(){
        
        cout << "Preparing. . .\n";
        string word;
        
        ifstream file;
        file.open("sgb-words.txt", ifstream::in);
        
        uint32_t i = 0;
        while (getline(file, word)){
            
            Tile* tile_ptr = new Tile(word, i);
            
            //string* word_ptr = new string(word);
            //nextWord.push(tile_ptr);
            dictionary.insert(tile_ptr);
            possibleGuesses.insert(tile_ptr);
 
            // Populate map
            for (int j = 0; j < 5; ++j){
                
                char letter = word[j];
                
                // whole set
                map[letter][0].insert(tile_ptr);
                
                // index set
                map[letter][j + 1].insert(tile_ptr);
                
            }
            
            i++;
            
        }
        
        cout << "Ready" << endl;
        
    } //readInDict
    
    void fillSet(string guess, vector<int> yellow, vector<int> green){
        
        unordered_set<int> grays ( {0, 1, 2, 3, 4} );
        
        for (auto x : green){

            // set<Tile*, Comp_s> tempSet;
            // guessedLetters.insert(x);
            // set_difference(possibleGuesses.begin(), possibleGuesses.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(tempSet, tempSet.begin()));
            // possibleGuesses = tempSet;
            
            // Narrow dictionary down to only words with letter at correct index
            set_intersection(dictionary.begin(), dictionary.end(), map[guess[x]][x + 1].begin(), map[guess[x]][x + 1].end(), inserter(working_set, working_set.begin()), Comp_s());

            dictionary = working_set;
            working_set.clear();
            grays.erase(x);
        }
            
        for (auto x : yellow){

            // set<Tile*, Comp_s> tempSet;
            // guessedLetters.insert(x);
            // set_difference(possibleGuesses.begin(), possibleGuesses.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(tempSet, tempSet.begin()));
            // possibleGuesses = tempSet;

            // Narrow dictionary down by elim words that have letter at spec index
            set_difference(dictionary.begin(), dictionary.end(), map[guess[x]][x + 1].begin(), map[guess[x]][x + 1].end(), inserter(working_set, working_set.begin()), Comp_s());
            
            dictionary = working_set;
            working_set.clear();
            
            // Narrow dict down to only words with letter at other indices
            set_intersection(dictionary.begin(), dictionary.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(working_set, working_set.begin()), Comp_s());
            
            dictionary = working_set;
            working_set.clear();
            grays.erase(x);
        }
            
        for (auto x : grays){

            // set<Tile*, Comp_s> tempSet;
            // guessedLetters.insert(x);
            // set_difference(possibleGuesses.begin(), possibleGuesses.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(tempSet, tempSet.begin()));
            // possibleGuesses = tempSet;
            
            // Narrow down dictionary by eliminating gray letters
            set_difference(dictionary.begin(), dictionary.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(working_set, working_set.begin()), Comp_s());
            
            dictionary = working_set;
            working_set.clear();
            
        }
        
    }

    // string nextGuess(){

    //     // words that do not contain any guessed letters
    //     // guessedLetters set
    //     // possibleGuesses set, set_difference for each letter guessed

    // }
    
    void prompt(){
        
        working_set.clear();
        
        for (int i = 0; i < 6; ++i){
            
            string guess;
            int yellow = -2;
            int green = -2;
            vector<int> g;
            vector<int> y;
            
            cout << "---------" << endl;
            cout << "Enter guess #" << i + 1 << ": ";
            cin >> guess;
            
            if (guess.length() != 5) {
                
                cerr << "Error: incorrect word length" << endl;
                exit(1);
                
            }
            
            cout << "Enter indices of green tiles, seperated by spaces, end with -1: " << endl;
            while (cin >> green){
                if (green == -1) { break; }
                g.push_back(green);
            }
            
            cout << "Enter indices of yellow tiles seperated by spaces, end with -1: " << endl;
            while (cin >> yellow){
                if (yellow == -1) { break; }
                y.push_back(yellow);
            }
            
            fillSet(guess, y, g);
            
            // If set size < 10 print words
            // cout << "dict size: " << dictionary.size() << endl;
            
            if (dictionary.size() == 1){
                
                cout << "Answer: ";
                for (auto word : dictionary){
                    cout << word->word << endl;
                }
                return;
                
            }

            // cout << "Suggested next guess: ";

            // priority_queue<Tile*, vector<Tile*>, Comp_o> nextWord(possibleGuesses.begin(), possibleGuesses.end());

            // size_t yh = nextWord.size();
            // int tel = nextWord.size();
            // for (int i = 0; i < tel; ++i){
            //     cout << nextWord.top()->word << endl;
            //     nextWord.pop();
            // }
            //cout << nextWord.top()->word << endl;
            // cout << yh << endl;
            


            
            if (dictionary.size() <= 10){
                cout << "Possible words:" << endl;
                
                for (auto word : dictionary){
                    
                    cout << word->word << endl;
                    
                }
            }
            
        }
        
    }
    
};

int main(int argc, char *argv[]) {
    
    ios_base::sync_with_stdio(false);
    //xcode_redirect(argc, argv);
    
    Wordle p;
    
    p.get_options(argc, argv);
    p.readInDict();
    p.prompt();
    
    return 0;
}
