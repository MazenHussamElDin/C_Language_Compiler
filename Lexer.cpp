#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "Htable.cpp"
#include "Token.cpp"

HashTable hashTable;

// Function to read file contents into string buffer
void read_file(std::string filepath, std::string& buffer) {
 std::ifstream file(filepath);
 std::string line;
 while (getline(file, line)) {
 buffer.push_back('\n');
 buffer += line;
 }
}

bool is_reserved(string identifier) {

  int i = hashTable.hashFunction(identifier);
  if(hashTable.search(identifier)){
    return true;
  }
  return false;

}

int main() {

  string buf;
  read_file("Trial.txt", buf);
   
  vector<string> keywords = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double",
                                "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
                                "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
                                "union", "unsigned", "void", "volatile", "while", "nullptr"};

  // Insert keywords into the hash table
  for (const string& keyword : keywords) {
        hashTable.insert(keyword);
  }

  //test buffer
  cout<<buf << endl;
  //test hashtable
  cout<< hashTable.size << endl;

  //test is reserved

  bool is = is_reserved("short");
  cout << " short is: "<< is << endl;
  is = is_reserved("hi");
  cout << " hi is: "<< is << endl;
  is = is_reserved("void");
  cout << " void is: "<< is << endl;


  return 0;
}
