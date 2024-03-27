#include <iostream>
#include <string>
#include <vector>

using namespace std;

class HashTable {
private:
    static const int TABLE_SIZE = 50; 

public :

    vector<string> table[TABLE_SIZE];
    int size=0;

    int hashFunction(const string& word) {
        int hash = 0;
        for (char c : word) {
            hash += c;
        }
        return hash % TABLE_SIZE;
    }

public:
    void insert(const string& word) {
        int index = hashFunction(word);
        for (const string& entry : table[index]) {
            if (entry == word) {
                return; // Word already exists, no need to insert again
            }
        }
        table[index].push_back(word);
        size++;
    }

    bool search(const string& word) {
        int index = hashFunction(word);
        for (const string& entry : table[index]) {
            if (entry == word) {
                return true; // Found the word
            }
        }
        return false; // Word not found
    }

    
};

