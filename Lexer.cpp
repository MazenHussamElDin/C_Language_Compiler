#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "Htable.cpp"
#include "Token.cpp"
#include <cassert>
#include <string.h>

HashTable hashTable;

using namespace std;

// Function to read file contents into string buffer
void read_file(std::string filepath, std::string& buffer) {
    std::ifstream file(filepath);
    std::string line;
    while (getline(file, line)) {
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
int add_to_symbol_table(std::string token_name, int offset) {
int index = 0;
 for (auto& symbol : symbol_table) {
 if (token_name == symbol.first) {
 symbol.second.push_back(offset);
 return index;
 }
 index++;
 }
 symbol_table.push_back({ token_name, { offset } });
 return index;
 }

 std::ostream& operator<<(std::ostream& out, const symbol_table_t& st) {
 std::string str = "\n\nSYMBOL TABLE\n\n";
 int index = 0;
 for (auto const symbol : symbol_table) {
 str += std::to_string(index++) + "- Symbol: " + symbol.first + ", at offset(s): ";
 for (auto offset : symbol.second) str += std::to_string(offset) + " ";
 str += "\n";
 }
 out << str;
 return out;
 }


bool get_next_token_regex(std::string& buffer, token*& token) {



  //EDIT WITHOUT DECIMAL
  static std::regex all_regexx(

    "//(.* \r\n)?"                          // Single line comments
    "|/\\*.*?\\*/"                  // Block comments
    "|(_+|[a-zA-Z])\\w*"           // strings
    "|\".*?\""                    // any text in string 
    "|(\\+\\+|--)"
    "|(>|<|==|>=|<=)" 
    "|(=|\\*=|%=|\\+=|-=|/=)"  
    "|(\\+|-|\\*|/|%)"
    "|(&&|\\|\\||!|<<|>>)"
    "|(&|\\||~)"
    "|(\\*|&|->)"
    "|\\."
    "|\\?"
    "|\\(|\\)"
    "|;"
    "|:"
    "|\\{|\\}"
    "|\\[|\\]"
    "|0[xX][0-9a-fA-F]+"                            //Hex
    "|0[0-7]+"                                     //Octal
    "|0[bB][01]+"                                 //Binary
                                                 //"|(\\+|-)?(0|[1-9][0-9]*)"
    "|[-+]?[0-9]*(\\.[0-9]+)?([eE][-+]?[0-9]+)?"
    "|."// floating point 
  );
  

	static std::sregex_iterator my_regex_iterator(
		buffer.begin(),
		buffer.end(),
		all_regexx
	);


	static std::sregex_iterator end;

	while (my_regex_iterator != end) {
		string match = my_regex_iterator->str();
		int lexeme_begin = my_regex_iterator->position();
		++my_regex_iterator;

 if (match == "++") 
			  return token->type = INCREMENT_OP;

      else if (match == "--")
        return token->type = DECREMENT_OP;

      else if (match == "&&")
        return token->type = LOGICAL_AND_OP;

      else if (match == "||")
        return token->type = LOGICAL_OR_OP;

      else if (match == "==")
        return token->type= EQUALS_OP;

      else if(match=="<=")
        return token->type= SMALLER_THAN_EQ_OP;

      else if(match==">=")
        return token->type= GREATER_THAN_EQ_OP;

      else if(match=="<<")
        return token->type= SHIFT_LEFT_OP;

      else if(match==">>")
        return token->type= SHIFT_RIGHT_OP;

      else if(match=="*=")
        return token->type= MUL_EQUAL_OP;

      else if(match=="/=")
        return token->type= DIV_EQUAL_OP;

      else if(match=="+=")
        return token->type= ADD_EQUAL_OP;

      else if(match=="-=")
        return token->type= SUB_EQUAL_OP;

      else if(match=="%=")
        return token->type= MOD_EQUAL_OP;

		if (match.length() == 1) {

			switch (match[0]) { // We can't switch-case on a string, so we get the first character in it
			case '<': return token->type = SMALLER_THAN_OP;
			case '>': return token->type = GREATER_THAN_OP;
			case '+': return token->type = ADD_OP;
			case '-': return token->type = SUB_OP;
			case '*': return token->type = ASTERISK_OP;
			case '/': return token->type = DIV_OP;
      case '=': return token->type = ASSIGN_OP;
			case '%': return token->type = REM_OP;
      case '!': return token->type = LOGICAL_NOT_OP;
      case '&': return token->type = BITWISE_AND_OP;
      case '|': return token->type = BITWISE_OR_OP;
      case '~': return token->type = BITWISE_NOT_OP;
      case '.': return token->type = MEMBER_OP;
      case '?': return token->type = TERNARY_OP;
      case '(': return token->type = LEFT_BRACKET;
      case ')': return token->type = RIGHT_BRACKET;
			case ';': return token->type = SEMICOLON;
      case ':': return token->type = COLON_OP;  
      case ',': return token->type = COMMA_OP;
      case '[': return token->type = LEFT_SQUARE_BRACKET;
      case ']': return token->type = RIGHT_SQUARE_BRACKET;
      case '{': return token->type = LEFT_CURLY_BRACKET;
      case '}': return token->type = RIGHT_CURLY_BRACKET;
			case ' ': continue;
			case '\n': continue;
			case '\t': continue;
			case EOF: return token->type = END_OF_FILE;
      //default: { return token->type = ERROR;}
			}

		}
    

    if (std::regex_match(match, std::regex("(_+|[a-zA-Z])\\w*"))) {
        token->name=match;
			if(hashTable.search(match))
        return token -> type = RESERVED_KW; 
      else 
        {token -> symbol_table_index = add_to_symbol_table(match, lexeme_begin);
        return token -> type = IDENTIFER;}

		}
 
 if (std::regex_match(match, std::regex("\".*?\""))) {
			token->value= match;
      
			return token->type = STRING_LITERAL;
		}
    
if (std::regex_match(match, std::regex("//(.*\r\n)?"))) {
    token->name = match;
    return token->type = SINGLE_LINE_COMMENT;
    
}


 if (std::regex_match(match, std::regex("/\\*.*?\\*/"))) {
    token->name = match;
    return token->type = BLOCK_COMMENT;
    
}

  if (std::regex_match(match, std::regex("[-+]?[0-9]+(\\.[0-9]+)?([eE][-+]?[0-9]+)?"))) {
    token->value = match;
    
    for (int i = 0 ; i<match.length() ; i++){
      if (match[i] == 'e' || match[i] == 'E')
        return token->type = EXP_NUMBER;
      
    }

    for (int i = 0 ; i<match.length() ; i++){
       if (match[i] == '.'){
        return token->type = FLOATING_POINT_NUMBER;
      }
    }
 
 return token->type = DEC_NUMBER;


    
}

    if(regex_match(match, regex("0[xX][0-9a-fA-F]+"))){ //Hex
      token->value = match.c_str();
      return token->type = HEX_NUMBER;
    } else if(regex_match(match, regex("0[0-7]+"))){ //Octal
      token->value = match.c_str();
      return token->type = O_NUMBER;
    } else if(regex_match(match, regex("0[bB][01]+"))){ //Binary
      token->value = match.c_str();
      return token->type = BIN_NUMBER;}



   if (std::regex_match(match, std::regex("."))) {
    token->name = match;
    return token->type = UNKNOWN;

  }

   
    
}
  return false;
}


bool get_next_token(std::string& buffer, token*& token) {

  token = new ::token();
  return get_next_token_regex(buffer, token);

}


int main() {

  // token* token;
  string buf;
  read_file("C:/Users/osha/Desktop/comp/C_Language_Compiler/Trial.txt", buf);
   
  vector<string> keywords = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double",
                                "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
                                "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
                                "union", "unsigned", "void", "volatile", "while", "nullptr"};

  // Insert keywords into the hash table
  for (const string& keyword : keywords) {
        hashTable.insert(keyword);
  }
  

  token* tok;
  std::vector<token*> tokens;
  while (get_next_token(buf, tok)) tokens.push_back(tok);

  // Print out the tokens and symbol table
  for (auto token : tokens){
    cout << *token << std::endl;
    
  }

  for (auto token : symbol_table){
    cout << token.first << " occurs at: ";
    for (auto tok : token.second){
      cout << tok << " ";
    }
    cout << endl;
  }

  return 0;
}
