#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

// Function to read file contents into string buffer
void read_file(std::string filepath, std::string& buffer);

// List of reserved keywords to compare identifiers against
std::vector<std::string> reserved_keywords = {
    "if", "then", "else", "end", "repeat", "until", "read", "write"
};
bool is_reserved(std::string identifier);

// Token stuff
class token;
std::ostream& operator<<(std::ostream& out, const token& token);

// Symbol table stuff
typedef std::vector<std::pair<std::string, std::vector<int>>> symbol_table_t;
symbol_table_t symbol_table;
int add_to_symbol_table(std::string token_name, int offset);
std::ostream& operator<<(std::ostream& out, const symbol_table_t& st);

// The core function of the scanner
int get_next_token(std::string& buffer, token*& token, bool reset = false);

// Simple demonstration of scanner functionality
#if defined(SCAN_ONLY)
int main(int argc, char const* argv[]) {

    // Read your input
    std::string buf;
    std::cout << "\nNow reading the file: " << argv[1] << std::endl;
    read_file(argv[1], buf);

    #if defined(DC)
    std::cout << "\nDIRECT-CODED IMPLEMENTATION OUTPUT\n" << std::endl;
    #elif defined(TD)
    std::cout << "\nTABLE-DRIVEN IMPLEMENTATION OUTPUT\n" << std::endl;
    #elif defined(RGX)
    std::cout << "\nREGEX IMPLEMENTATION OUTPUT\n" << std::endl;
    #endif

    // Tokenize it
    token* tok;
    std::vector<token*> tokens;
    while (get_next_token(buf, tok) > 0) tokens.push_back(tok);

    // Print out the tokens and symbol table
    for (auto token : tokens) std::cout << *token << std::endl;
    std::cout << symbol_table << std::endl;

    // And that's it!
    return 0;

}
#endif

// Reading the entire file at once! Very impractical! Should buffer it.
void read_file(std::string filepath, std::string& buffer) {

    std::ifstream file(filepath);
    std::string line;
    while (getline(file, line)) {
        buffer.push_back('\n');
        buffer += line;
    }

}

// Linear search! Very impractically slow! Should use a better data structure.
bool is_reserved(std::string identifier) {

    for (auto kw : reserved_keywords) {
        if (identifier == kw) {
            return true;
        }
    }
    return false;

}

enum token_type {
    END_OF_FILE = 0,
    RESERVED_KW, IDENTIFIER, NUMBER,
    ADD_OP, SUB_OP, MUL_OP, DIV_OP,
    EQUALS_OP, SMALLER_THAN_OP,
    ASSIGN_OP,
    LEFT_BRACKET, RIGHT_BRACKET, SEMICOLON,
    BLOCK_COMMENT,
    ERROR
};

class token {

public:
    enum token_type type;
    std::string name;
    int value;
    int symbol_table_index;
    std::string typestr() {
        switch (type) {
            case END_OF_FILE: return "END_OF_FILE";
            case RESERVED_KW: return "RESERVED_KW";
            case IDENTIFIER: return "IDENTIFIER";
            case NUMBER: return "NUMBER";
            case ADD_OP: return "ADD_OP";
            case SUB_OP: return "SUB_OP";
            case MUL_OP: return "MUL_OP";
            case DIV_OP: return "DIV_OP";
            case EQUALS_OP: return "EQUALS_OP";
            case SMALLER_THAN_OP: return "SMALLER_THAN_OP";
            case ASSIGN_OP: return "ASSIGN_OP";
            case LEFT_BRACKET: return "LEFT_BRACKET";
            case RIGHT_BRACKET: return "RIGHT_BRACKET";
            case SEMICOLON: return "SEMICOLON";
            case BLOCK_COMMENT: return "BLOCK_COMMENT";
            case ERROR: return "ERROR";
            default: return "UNDEFINED";
        }
    }

};

std::ostream& operator<<(std::ostream& out, const token& token) {

    std::string str;
    str += "<";
    switch (token.type) {

        case RESERVED_KW:
            str += "reserved keyword, name = " + token.name;
            break;

        case IDENTIFIER:
            str += "identifier, name = " + token.name +
                ", symbol table index #" + std::to_string(token.symbol_table_index);
            break;

        case NUMBER:
            str += "number, value = " + std::to_string(token.value);
            break;

        case ADD_OP:
            str += "addition arithmetic operator";
            break;

        case SUB_OP:
            str += "subtract arithmetic operator";
            break;

        case MUL_OP:
            str += "multiply arithmetic operator";
            break;

        case DIV_OP:
            str += "division arithmetic operator";
            break;

        case EQUALS_OP:
            str += "equality comparison operator";
            break;

        case SMALLER_THAN_OP:
            str += "smaller-than comparison operator";
            break;

        case ASSIGN_OP:
            str += "assignment operator";
            break;

        case LEFT_BRACKET:
            str += "left bracket";
            break;

        case RIGHT_BRACKET:
            str += "right bracket";
            break;

        case SEMICOLON:
            str += "semicolon";
            break;

        case BLOCK_COMMENT:
            str += "block comment (to be ignored), content = " + token.name;
            break;

        case END_OF_FILE:
            str += "end-of-file";
            break;

        case ERROR:
            str += "erroneous token, content = " + token.name;
            break;

        default:
            str += "unexpected token";
            break;

    }

    str += ">";
    out << str;
    return out;

}

// Hash table please! Very slow linear string equality search!
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

        str += std::to_string(index++) + "- Symbol: " + symbol.first;
        str += ", at offset(s): ";

        for (auto offset : symbol.second)
            str += std::to_string(offset) + " ";

        str += "\n";

    }

    out << str;
    return out;

}

// Two possible implementations for get_next_token

// 1. Direct-coded (get_next_token_dc)
int get_next_token_dc(std::string& buffer, token*& token, bool reset);

// 2. Table-driven (get_next_token_td)
int get_next_token_td(std::string& buffer, token*& token, bool reset);

// 3. In regex (get_next_token_regex)
int get_next_token_regex(std::string& buffer, token*& token, bool reset);

int get_next_token(std::string& buffer, token*& token, bool reset) {

    token = new ::token();

    #if defined(DC)
    return get_next_token_dc(buffer, token, reset);
    #elif defined(TD)
    return get_next_token_td(buffer, token, reset);
    #elif defined(RGX)
    return get_next_token_regex(buffer, token, reset);
    #else
    return -3; // No implementation selected
    #endif

}

int get_next_token_dc(std::string& buffer, token*& token, bool reset) {

    // This will be our forward pointer (index) that we traverse the buffer with
    static int forward = 0;
    if (reset) forward = 0;

    // Forward pointer reached the end of the buffer, return 0 for successful completion of scanner
    if (forward >= buffer.length()) return 0;

    // The lexemeBegin pointer (index) that tells us where the current lexeme begins in the buffer
    int lexeme_begin = forward;

    // Our states
    enum state_type { START, INASSIGN, INCOMMENT, INNUM, INID, DONE };
    state_type current_state = START;

    // The character at the forward index currently
    char current_char;

    while (true) {

        current_char = buffer[forward++];
        // current_char = *(buffer.c_str() + forward++); 
        // c_str() gets char pointer of std::string
        switch (current_state) {
            case START:
                if (isdigit(current_char)) current_state = INNUM;
                else if (isalpha(current_char)) current_state = INID;
                else if (current_char == ':') current_state = INASSIGN;
                else if (current_char == '{') current_state = INCOMMENT;
                else if ((current_char == ' ') || (current_char == '\t') || (current_char == '\n')) lexeme_begin++;
                else {
                    // If none of the above cases, then we're matching a one-character-long token
                    switch (current_char) {
                        case '=': token->name = current_char; token->type = EQUALS_OP; return forward;
                        case '<': token->name = current_char; token->type = SMALLER_THAN_OP; return forward;
                        case '+': token->name = current_char; token->type = ADD_OP; return forward;
                        case '-': token->name = current_char; token->type = SUB_OP; return forward;
                        case '*': token->name = current_char; token->type = MUL_OP; return forward;
                        case '/': token->name = current_char; token->type = DIV_OP; return forward;
                        case '(': token->name = current_char; token->type = LEFT_BRACKET; return forward;
                        case ')': token->name = current_char; token->type = RIGHT_BRACKET; return forward;
                        case ';': token->name = current_char; token->type = SEMICOLON; return forward;
                        case '\0':
                        case EOF: token->type = END_OF_FILE; return 0;
                        default:
                            token->name = buffer.substr(lexeme_begin, forward - lexeme_begin);
                            token->type = ERROR;
                            return forward;
                    }
                }
                break;
            case INCOMMENT:
                if (current_char == EOF || current_char == '\0') {
                    token->type = END_OF_FILE;
                    return -1; // An unterminated comment
                } else if (current_char == '}') {
                    // No lookahead to undo
                    token->name = buffer.substr(lexeme_begin, forward - lexeme_begin);
                    token->type = BLOCK_COMMENT;
                    return forward;
                    // If we want to skip returning the comment altogether, then simply do:
                    // current_state = START;
                    // lexeme_begin = forward;
                }
                break;
            case INASSIGN:
                if (current_char == '=') {
                    token->name = ":=";
                    token->type = ASSIGN_OP;
                    return forward;
                } else {
                    token->name = buffer.substr(lexeme_begin, forward - lexeme_begin);
                    forward--; // undo lookahead
                    token->type = ERROR;
                    return forward;
                }
                break;
            case INNUM:
                if (!isdigit(current_char)) {
                    forward--; // undo lookahead
                    token->value = atoi(buffer.substr(lexeme_begin, forward - lexeme_begin).c_str());
                    token->type = NUMBER;
                    return forward;
                }
                break;
            case INID:
                if (!isalpha(current_char)) {
                    forward--; // undo lookahead
                    // Can check here for length of identifier, to enforce 40 char limit
                    token->name = buffer.substr(lexeme_begin, forward - lexeme_begin);
                    if (is_reserved(token->name)) {
                        token->type = RESERVED_KW;
                        return forward;
                    } else {
                        token->symbol_table_index = add_to_symbol_table(token->name, lexeme_begin);
                        token->type = IDENTIFIER;
                        return forward;
                    }
                }
                break;
        }

    }

    return -1;

}

int get_next_token_td(std::string& buffer, token*& token, bool reset) {

    // Not implemented..
    return -2;

}

int get_next_token_regex(std::string& buffer, token*& token, bool reset) {

    static std::regex my_regex("\\{[^\\{\\}]*\\}|[0-9]+|[a-zA-Z]+|:=|=|<|\\+|-|\\*|/|\\(|\\)|;|[ \n\t]+|.");

    static std::sregex_iterator my_regex_iterator(
        buffer.begin(),
        buffer.end(),
        my_regex
    );
    if (reset) my_regex_iterator = std::sregex_iterator(
        buffer.begin(),
        buffer.end(),
        my_regex
    );

    static std::sregex_iterator end;

    while (my_regex_iterator != end) {

        std::string match = my_regex_iterator->str();
        int lexeme_begin = my_regex_iterator->position();
        ++my_regex_iterator; // Now "my_regex_iterator->position()" represents forward pointer
        // std::cout << "DEBUG: " << match << std::endl;
        if (match.length() == 1) {
            switch (match[0]) { // We can't switch-case on a string, so we get the first character in it
                case '=': token->name = match[0]; token->type = EQUALS_OP; return my_regex_iterator->position();
                case '<': token->name = match[0]; token->type = SMALLER_THAN_OP; return my_regex_iterator->position();
                case '+': token->name = match[0]; token->type = ADD_OP; return my_regex_iterator->position();
                case '-': token->name = match[0]; token->type = SUB_OP; return my_regex_iterator->position();
                case '*': token->name = match[0]; token->type = MUL_OP; return my_regex_iterator->position();
                case '/': token->name = match[0]; token->type = DIV_OP; return my_regex_iterator->position();
                case '(': token->name = match[0]; token->type = LEFT_BRACKET; return my_regex_iterator->position();
                case ')': token->name = match[0]; token->type = RIGHT_BRACKET; return my_regex_iterator->position();
                case ';': token->name = match[0]; token->type = SEMICOLON; return my_regex_iterator->position();
                case ' ': continue;
                case '\n': continue;
                case '\t': continue;
                case '\0':
                case EOF: token->type = END_OF_FILE; return 0;
            }
        }
        if (std::regex_match(match, std::regex("\\{[^\\{\\}]+\\}"))) {
            token->name = match;
            token->type = BLOCK_COMMENT;
            return my_regex_iterator->position();
            // If we want to skip comments, we can do:
            // continue;
        } else if (match == ":=") {
            token->name = match;
            token->type = ASSIGN_OP;
            return my_regex_iterator->position();
        } else if (std::regex_match(match, std::regex("[0-9]+"))) {
            token->value = atoi(match.c_str());
            token->type = NUMBER;
            return my_regex_iterator->position();
        } else if (std::regex_match(match, std::regex("[a-zA-Z]+"))) {
            token->name = match;
            if (is_reserved(token->name)) {
                token->type = RESERVED_KW;
                return my_regex_iterator->position();
            } else {
                token->symbol_table_index = add_to_symbol_table(match, lexeme_begin);
                token->type = IDENTIFIER;
                return my_regex_iterator->position();
            }
        } else if (std::regex_match(match, std::regex("[ \n\t]+"))) {
            continue; // whitespace
        } else {
            // the catch-all case
            token->name = match;
            token->type = ERROR;
            return my_regex_iterator->position();
        }

    }
    return 0;

}
