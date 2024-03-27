#include <iostream>
#include <string>

using namespace std;


enum token_type {
END_OF_FILE = 0, ERROR,
RESERVED_KW, IDENTIFER, NUMBER,
ADD_OP, SUB_OP, MUL_OP, DIV_OP,
EQUALS_OP, SMALLER_THAN_OP,
ASSIGN_OP,
LEFT_BRACKET, RIGHT_BRACKET, SEMICOLON,
BLOCK_COMMENT
};


class token {

public:

    enum token_type type;
    std::string name;
    int value;
    int symbol_table_index;
    
};
