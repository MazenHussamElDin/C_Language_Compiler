#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "Htable.cpp"

// Function to read file contents into string buffer
 void read_file(std::string filepath, std::string& buffer) {
 std::ifstream file(filepath);
 std::string line;
 while (getline(file, line)) {
 buffer.push_back('\n');
 buffer += line;
 }
 }

bool is_reserved(std::string identifier) {
    // Check if the identifier matches any of the reserved keywords
  return false;
}

int main() {

   string buf;
   read_file("Trial.txt", buf);
   HashTable hashTable;

    vector<string> keywords = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double",
                                "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
                                "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
                                "union", "unsigned", "void", "volatile", "while", "nullptr"};

    // Insert keywords into the hash table
    for (const string& keyword : keywords) {
        hashTable.insert(keyword);
    }

    cout<<buf;

    cout<< hashTable.size;

    return 0;
}
