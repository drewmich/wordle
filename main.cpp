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
#include <unordered_set>
#include <unordered_map>
#include "xcode_redirect.hpp"
#include "getopt.h"

using namespace std;

class Wordle{
public:
    set<string> dictionary;
    set<string> working_set;
    unordered_map<char, unordered_map<int, set<string> > > map;
    
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
        
        while (getline(file, word)){
            
            dictionary.insert(word);
 
            // Populate map
            for (int j = 0; j < 5; ++j){
                
                char letter = word[j];
                
                // whole set
                map[letter][0].insert(word);
                
                // index set
                map[letter][j + 1].insert(word);
                
            }
            
        }
        
        cout << "Ready" << endl;
        
    } //readInDict
    
    void fillSet(string guess, vector<int> yellow, vector<int> green){
        
        unordered_set<int> grays = {0, 1, 2, 3, 4};
        
        for (auto x : green){
            
            // Narrow dictionary down to only words with letter at correct index
            set_intersection(dictionary.begin(), dictionary.end(), map[guess[x]][x + 1].begin(), map[guess[x]][x + 1].end(), inserter(working_set, working_set.begin()));

            dictionary = working_set;
            working_set = {};
            grays.erase(x);
        }
            
        for (auto x : yellow){

            // Narrow dictionary down by elim words that have letter at spec index
            set_difference(dictionary.begin(), dictionary.end(), map[guess[x]][x + 1].begin(), map[guess[x]][x + 1].end(), inserter(working_set, working_set.begin()));
            
            dictionary = working_set;
            working_set = {};
            
            // Narrow dict down to only words with letter at other indices
            set_intersection(dictionary.begin(), dictionary.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(working_set, working_set.begin()));
            
            dictionary = working_set;
            working_set = {};
            grays.erase(x);
        }
            
        for (auto x : grays){
            
            // Narrow down dictionary by eliminating gray letters
            set_difference(dictionary.begin(), dictionary.end(), map[guess[x]][0].begin(), map[guess[x]][0].end(), inserter(working_set, working_set.begin()));
            
            dictionary = working_set;
            working_set = {};
            
        }
        
    }
    
    void prompt(){
        
        working_set = {};
        
        for (int i = 0; i < 6; ++i){
            
            string guess;
            int yellow = -2;
            int green = -2;
            vector<int> g = {};
            vector<int> y = {};
            
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
            cout << "dict size: " << dictionary.size() << endl;
            
            if (dictionary.size() == 1){
                
                cout << "Answer: ";
                for (auto word : dictionary){
                    cout << word << endl;
                }
                return;
                
            }
            
            if (dictionary.size() <= 10){
                cout << "Possible words:" << endl;
                
                for (auto word : dictionary){
                    
                    cout << word << endl;
                    
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
