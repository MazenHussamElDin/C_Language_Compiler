#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "Htable.cpp"
#include "Token.cpp"
#include <cassert>

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

	static std::regex all_operators(
		"\\+\\+|--"                // Unary arithmetic operators (++, --)
		"|\\+|-|\\*|/|%"           // Binary arithmetic operators (+, -, *, /, %)
		"|>|<|==|>=|<="            // Binary relational operators (>, <, ==, >=, <=)
		"|=|\\*=|%=|\\+=|-=|/="    // Binary assignment operators (=, *=, %=,/=, +=, -=)
		"|&&|\\|\\||!|<<|>>"       // Binary logical operators (&&, ||, !, <<, >>)
		"|&|\\||~"                 // Binary bitwise operators (&, |, ~)
		"|\\*|&|->"                // Pointer operations (*, &, ->)
		"|\\."                     // Member operations (.)
		"|\\?"                     // Ternconditional operator ()
    "|\\(|\\)"                 // Parentheses
    "|;"                       // Semicolon
    "|:"
    "|\\{|\\}"                  // Braces
    "\\[|\\]"                  // Brackets
	);

	static std::sregex_iterator my_regex_iterator(
		buffer.begin(),
		buffer.end(),
		all_operators
	);
	static std::sregex_iterator end;
	while (my_regex_iterator != end) {
		std::string match = my_regex_iterator->str();
		int lexeme_begin = my_regex_iterator->position();
		++my_regex_iterator;

		if (match.length() == 1) {
			switch (match[0]) { // We can't switch-case on a string, so we get the first character in it
			case '=': {cout << "=";  return token->type = ASSIGN_OP;};
			case '<': return token->type = SMALLER_THAN_OP;
			case '>': return token->type = GREATER_THAN_OP;
			case '+': return token->type = ADD_OP;
			case '-': return token->type = SUB_OP;
			case '*': return token->type = ASTERISK_OP;
			case '/': return token->type = DIV_OP;
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
      default: {cout << "Error";  return token->type = ERROR;}
			}
		}
    if (std::regex_match(match, std::regex("\\{[^\\{\\}]+\\}"))) {
			token->name = match;
			return token->type = BLOCK_COMMENT;
		}
    if (match.length() == 2){
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

  }
   
  }
  return false;
}

int main() {

  


  // token* token;
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
     std::vector<token*> tokens;
    token* my_token = new token();
    while(get_next_token_regex(buf, my_token)){
      tokens.push_back(my_token);

    }
    for (auto token : tokens) std::cout << *token << std::endl;
    

    cout<< endl;

    

// bool x= get_next_token_regex(buf, token);
// cout << x;
 // Tokenize it
//  token* tok;
//  std::vector<token*> tokens;
//  while (get_next_token_regex(buf, tok)) tokens.push_back(tok);
//  // Print out the tokens and symbol table
//  for (auto token : tokens) std::cout << *token << std::endl;
//  std::cout << symbol_table << std::endl;

  // //test buffer
  // cout<<buf << endl;
  // //test hashtable
  // cout<< hashTable.size << endl;

  // //test is reserved

  // bool is = is_reserved("short");
  // cout << " short is: "<< is << endl;
  // is = is_reserved("hi");
  // cout << " hi is: "<< is << endl;
  // is = is_reserved("void");
  // cout << " void is: "<< is << endl;


  return 0;
}
