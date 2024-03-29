#include <iostream>
#include <string>
#include <vector>
using namespace std;


enum token_type {
    END_OF_FILE = 0, ERROR,
    RESERVED_KW, IDENTIFER, NUMBER,
    //check this
    DEC_NUMBER, HEX_NUMBER, O_NUMBER, BIN_NUMBER,
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
    LEFT_CURLY_BRACKET,RIGHT_CURLY_BRACKET,LEFT_SQUARE_BRACKET,RIGHT_SQUARE_BRACKET, EXP_NUMBER, FLOATING_POINT_NUMBER,
    SINGLE_LINE_COMMENT
    
};


class token {

public:

    enum token_type type;
    std::string name;
    string value;
    int symbol_table_index;
    
};

 typedef std::vector<std::pair<std::string, std::vector<int>>> symbol_table_t;
  symbol_table_t symbol_table;

std::ostream& operator<<(std::ostream& out, const token& token) {
 std::string str;
 str += "<";
 switch (token.type) {

  case ASSIGN_OP:
  str += "equals operator";
  break;

  case RESERVED_KW:
  str += "keyword, value=" + token.name;
  break;

  case IDENTIFER:
  str += "identifier, Value= " + token.name;
  break;

  case DEC_NUMBER:
  str+="Decimal Number, Value= " + token.value;
  break;

case EXP_NUMBER:
  str+="Exponential Number, Value= " + token.value;
  break;

  case FLOATING_POINT_NUMBER:
  str+="Floating Point Number, Value= " + token.value;
  break;
 
  case HEX_NUMBER:
  str+="HEX Number, Value= " + token.value;
  break;

  case O_NUMBER:
  str+="OCT Number, Value= " + token.value;
  break;

  case BIN_NUMBER:
  str+="BIN Number, Value= " + token.value;
  break;

  case SEMICOLON:
  str += "semicolon";
  break;

  case LEFT_BRACKET:
  str += "left bracket";
  break;

  case RIGHT_BRACKET:
  str += "right bracket";
  break;

  case RIGHT_CURLY_BRACKET:
  str += "right curly bracket";
  break;

  case LEFT_CURLY_BRACKET:
  str += "left curly bracket";
  break;

  case LEFT_SQUARE_BRACKET:
  str += "left square bracket";
  break;

  case RIGHT_SQUARE_BRACKET:
  str += "right square bracket";
  break;

  case BLOCK_COMMENT:   
  str += "block comment to be ignored";
  break;

  case SINGLE_LINE_COMMENT:   
  str += "single line comment to be ignored";
  break;

  case ERROR:
  str += "error";
  break;

  case END_OF_FILE:
  str += "end of file";
  break;

 case ADD_OP :
  str += "addition operator";
  break;

  case SUB_OP : 
  str += "subtraction operator";
  break;

  case ASTERISK_OP :
  str += "multiplication operator";
  break;  

  case DIV_OP :
  str += "division operator";
  break;  

  case REM_OP :
  str += "remainder operator";  
  break;  

  case LOGICAL_NOT_OP :
  str += "logical not operator";
  break;

  case BITWISE_AND_OP : 
  str += "bitwise and operator";
  break;

  case BITWISE_OR_OP :
  str += "bitwise or operator";
  break;

  case BITWISE_NOT_OP :
  str += "bitwise not operator";
  break;

  case MEMBER_OP :
  str += "member operator";
  break;

  case TERNARY_OP :
  str += "ternary operator";
  break;

  case COLON_OP :
  str += "colon operator";
  break;

  case COMMA_OP :
  str += "comma operator";
  break;

case INCREMENT_OP :
  str += "increment operator";
  break;

case DECREMENT_OP :  
  str += "decrement operator";
  break;

case SHIFT_LEFT_OP :
  str += "shift left operator";
  break;

case SHIFT_RIGHT_OP :
  str += "shift right operator";
  break;

case ADD_EQUAL_OP :
  str += "addition assignment operator";
  break;

case SUB_EQUAL_OP :
  str += "subtraction assignment operator";
  break;

case MUL_EQUAL_OP :
  str += "multiplication assignment operator";
  break;

case DIV_EQUAL_OP :
  str += "division assignment operator";
  break;

case MOD_EQUAL_OP :
  str += "modulus assignment operator";
  break;

case EQUALS_OP :
  str += "equals operator";
  break;

case SMALLER_THAN_OP :
  str += "smaller than operator";
  break;

case SMALLER_THAN_EQ_OP :
  str += "smaller than or equal to operator";
  break;

case GREATER_THAN_OP :
  str += "greater than operator";
  break;  

case GREATER_THAN_EQ_OP :
  str += "greater than or equal to operator";
  break;

case LOGICAL_AND_OP :
  str += "logical and operator";
  break;

case LOGICAL_OR_OP :
  str += "logical or operator";
  break;



 

 }
 str += ">";
 out << str;
 return out;
 }

  
 