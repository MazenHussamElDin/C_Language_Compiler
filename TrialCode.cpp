#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

// Function to read file contents into string buffer
void read_file(std::string filepath, std::string& buffer);

// List of reserved keywords to compare identifiers against
std::vector<std::string> reserved_keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while", "nullptr"
};

bool is_reserved(std::string identifier) {
    // Check if the identifier matches any of the reserved keywords
    return std::find(reserved_keywords.begin(), reserved_keywords.end(), identifier) != reserved_keywords.end();
}

int main() {
    // Your lexer implementation goes here
    return 0;
}
