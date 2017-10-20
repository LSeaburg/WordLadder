/* Logan Seaburg
 *
 *I used lecture notes as well as the Stanford C++ libraries
 * and references
 *
 * --- Extras ---
 * when prompted to enter a filename, type 'non-real words' to
 * allow the first and last words to be words not dound in the dictionary
 * and type different lengths to allow the program to be able to change
 * one letter at a time
 */
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "hashset.h"
#include "queue.h"
#include "simpio.h"
#include "stack.h"
#include "strlib.h"

using namespace std;

void intro();
void readDictionary(HashSet<string>& dictionary, bool& nonRealWords, bool& differentLengths);
void enterWords(string& word1, string& word2, HashSet<string>& dictionary, bool nonRealWords, bool differentLengths);
bool valid(string word1, string word2, HashSet<string>& dictionary, bool nonRealWords, bool differentLengths);
void solve(string word1, string word2, HashSet<string>& dictionary, bool differentLengths);
HashSet<string> findNeighbors(string word, HashSet<string>&dictionary, bool differentLengths, string word2);
HashSet<string> extraNeighbors(string word, HashSet<string>&dictionary, string word2);
string replace(int index, char ch, string word);

int main() {
    HashSet<string> dictionary;
    string word1, word2;
    bool nonRealWords = false, differentLengths = false;
    intro();
    readDictionary(dictionary, nonRealWords, differentLengths);
    do {
        enterWords(word1, word2, dictionary, nonRealWords, differentLengths);
        if (! (word1 == "" || word2 == "") ) {
            solve(word1, word2, dictionary, differentLengths);
        }
    } while (! (word1 == "" || word2 == "") );
    cout << "Have a nice day." << endl;
    return 0;
}

//displays the introductory message for the user
void intro() {
    cout << "Welcome to CS 106B Word Ladder." << endl
         << "Please give me two English words, and I will change the" << endl
         << "first into the second by changing one letter at a time." << endl
         << "Additionally, you can type 'non-real words' to allow the starting" << endl
         << "and ending words to be strings like names, and 'different lengths' to" << endl
         << "let the program add and remove letters as long as it's only 1 at a time." << endl
         << endl;
    return;
}

/*Passes in a set of the dictionary that stores all of the words, also handels the
 * booleans that controls whether or not the extra features are on.
 * Collects the file name for the dictionary and stores the words into a set
 */
void readDictionary(HashSet<string>& dictionary, bool& nonRealWords, bool& differentLengths) {
    string fileName;
    string word;
    ifstream input;

    fileName = getLine ("Dictionary file name? ");
    while (! fileExists(fileName)) {
        if (toLowerCase(fileName) == "non-real words") {
            nonRealWords = !nonRealWords;
            if (nonRealWords) {
                cout << "Non-real starting and ending points now allowed." << endl;
            } else {
                cout << "Non-real starting and ending points disallowed." << endl;
            }
        } else if (toLowerCase(fileName) == "different lengths") {
            differentLengths = !differentLengths;
            if (differentLengths) {
                cout << "Staring and ending words can now be different lengths." << endl;
            } else {
                cout << "Starting and ending words can't be different lengths." << endl;
            }
        } else {
            cout << "Unable to open that file. Try again." << endl;
        }
        fileName = getLine ("Dictionary file name? ");
    }

    //store all of the words in the dictionary
    openFile(input, fileName);
    while (getline(input, word)) {
        dictionary.add(trim(word) );
    }
    input.close();
    return;
}

/* Simply collects the 2 words that the user enters, handels them, and
 * calls a function to see if they are valid. Passes the words as reference,
 * also needs acess to the dictionary and settings to pass to the valid function.
 */
void enterWords(string& word1, string& word2, HashSet<string>& dictionary, bool nonRealWords, bool differentLengths) {
    do {
        cout << endl;
        cout << "Word #1 (or Enter to quit): ";
        getline(cin, word1);
        word1 = toLowerCase(word1);
        if (word1 == "") {
            return;
        }
        cout << "Word #2 (or Enter to quit): ";
        getline(cin, word2);
        word2 = toLowerCase(word2);
        if (word2 == "") {
            return;
        }
    } while (! valid(word1, word2, dictionary, nonRealWords, differentLengths) );
    return;
}

/*Takes in 2 words and checks if they are valid input. Needs acess to the
 * dictionary and the settings so it knows what to check.
 */
bool valid(string word1, string word2, HashSet<string>& dictionary, bool nonRealWords, bool differentLengths) {
    if (word1 == word2) {
        cout << "The two words must be different." << endl;
        return false;
    } else if (! (word1.length() == word2.length()) && ! differentLengths) {
        cout << "The two words must be the same length." << endl;
        return false;
    } else if (! (dictionary.contains(word1) && dictionary.contains(word2) ) && (! nonRealWords) ) {
        cout << "The two words must be found in the dictionary." << endl;
        return false;
    }
    return true;
}

/*Takes in the 2 words, the dictionary, and the settings and solves the word ladder.
 * It looks for all possible ladders, and branches out, keeping all of them in a queue.
 * Each attemp is stored as a stack, and they all progress 1 at a time until the solution
 * has been reached or all attempts have failed.
 */
void solve(string word1, string word2, HashSet<string>& dictionary, bool differentLengths) {
    Queue<Stack<string> > answers;
    HashSet<string> triedWords, neighbors;
    Stack<string> stack;
    stack.add(word1);
    answers.enqueue(stack);
    while (! answers.isEmpty()) {
        stack = answers.dequeue();
        neighbors = findNeighbors(stack.peek(), dictionary, differentLengths, word2);
        for (string neighbor : neighbors) {
            if (! triedWords.contains(neighbor) ) {
                Stack<string> tempStack = stack;
                tempStack.add(neighbor);
                triedWords.add(neighbor);
                if (neighbor != word2) {
                    answers.enqueue(tempStack);
                } else {
                    cout << "A word ladder from " << word2 << " back to " << word1 << ":" << endl;
                    while (! tempStack.isEmpty() ) {
                        cout << tempStack.pop() << " ";
                    }
                    cout << endl;
                    return;
                }
            }
        }
    }
    cout << "No word ladder found from " << word2 << " back to " << word1 << "." << endl;
    return;
}

/*Takes in the word that it will look for neighbors for, and the dictionary, along with
 * other things for settings and the extras. It tries every letter in every place and sees if they
 * are words.
 */
HashSet<string> findNeighbors(string word, HashSet<string>& dictionary, bool differentLengths, string word2) {
    HashSet<string> neighbors;
    string tempWord;
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < word.length(); i++) {
        for(char&c : alphabet) {
            tempWord = replace(i, c, word);
            if (dictionary.contains(tempWord) || tempWord == word2) {
                neighbors.add(tempWord);
            }
        }
    }
    if (differentLengths) {
        neighbors += extraNeighbors(word, dictionary, word2);
    }
    return neighbors;
}

/*If words can change lengths, it will insert every letter into every space, and try to remove
 * every letter, testing if those create valid words.
 * This takes the original word, the dictionary to check, and word 2 in case it is an non-real
 * word (from the setting)
 */
HashSet<string> extraNeighbors(string word, HashSet<string>&dictionary, string word2) {
    HashSet<string> neighbors;
    string tempWord;
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i <= word.length(); i++) {
        for(char& c : alphabet) {
            tempWord = word;
            string str;
            str += c;
            tempWord.insert(i, str);
            if (dictionary.contains(tempWord) || tempWord == word2) {
                neighbors.add(tempWord);
            }
        }
    }
    for (int i = 0; i < word.length(); i++) {
        tempWord = word;
        tempWord.erase(tempWord.begin() + i);
        if (dictionary.contains(tempWord) || tempWord == word2) {
            neighbors.add(tempWord);
        }
    }
    return neighbors;
}

/*For a string word, it will replace the character at index i
 * with the char ch.
 */
string replace(int index, char ch, string word) {
    string temp;
    for(int i = 0; i < word.length(); i++) {
        if (i == index) {
            temp += ch;
        } else {
            temp += word[i];
        }
    }
    return temp;
}
