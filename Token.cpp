#include <iostream>
#include <string>
#include <vector>
using namespace std;


enum token_type {
    END_OF_FILE = 0, ERROR,
    RESERVED_KW, IDENTIFER, NUMBER,
    ADD_OP, SUB_OP, ASTERISK_OP, DIV_OP,
    EQUALS_OP, SMALLER_THAN_OP,
    SMALLER_THAN_EQ_OP, GREATER_THAN_OP, GREATER_THAN_EQ_OP, NOT_EQ_OP, BITWISE_AND_OP,
    BITWISE_OR_OP, BITWISE_NOT_OP,
    LOGICAL_AND_OP, LOGICAL_OR_OP, LOGICAL_NOT_OP,
    ASSIGN_OP, REM_OP,
    LEFT_BRACKET, RIGHT_BRACKET, SEMICOLON,
    BLOCK_COMMENT,COLON_OP,
    POINTER_DEC_OP,POINTER_REF_OP,POINTER_MEM_ACCESS_OP, MEMBER_OP, TERNARY_OP,INCREMENT_OP,DECREMENT_OP,
    COMMA_OP,SHIFT_LEFT_OP,SHIFT_RIGHT_OP,ADD_EQUAL_OP,SUB_EQUAL_OP,MUL_EQUAL_OP,DIV_EQUAL_OP,MOD_EQUAL_OP,
    LEFT_CURLY_BRACKET,RIGHT_CURLY_BRACKET,LEFT_SQUARE_BRACKET,RIGHT_SQUARE_BRACKET
    
};


class token {

public:

    enum token_type type;
    std::string name;
    int value;
    int symbol_table_index;
    
};

 typedef std::vector<std::pair<std::string, std::vector<int>>> symbol_table_t;
  symbol_table_t symbol_table;

std::ostream& operator<<(std::ostream& out, const token& token) {
 std::string str;
 str += "<";
 switch (token.type) {

  case EQUALS_OP:
  str += "equals operator";
    break;

  case ADD_OP:
 str += "addition arithmetic operator";
 break;

 case SEMICOLON:
    str += "semicolon";

 }
 str += ">";
 out << str;
 return out;
 }

  
 