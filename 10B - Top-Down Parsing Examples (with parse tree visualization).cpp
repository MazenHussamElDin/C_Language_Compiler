// Don't you ever dare include a CPP file like I am doing here
// I am your TA, so I do dangerous things! -- but you shouldn't
#include "scannertiny.cpp"

// A small addition for tokens that don't exist in the TINY scanner but I need for this example:
enum token_type_extra { COMMA = 100, LEFT_CURLY_BRACKET, RIGHT_CURLY_BRACKET };

// Nodes in the parse tree can be represented by this very very very minimal class
class parse_tree_node {
public:
    bool is_terminal;
    std::string label;
    std::deque<parse_tree_node*> children;
    parse_tree_node(std::string l, bool t) : label(l), is_terminal(t) {}
};

// A function to uniquely identify the node in graphviz dot style
std::ostream& operator<<(std::ostream& out, parse_tree_node* node) {
    out << "\"" << (void*) node << "\" [label=\"" << node->label << "\"]";
    return out;
}

// Global "buf" buffer to store text in
std::string buf;

// Global "tok" token to lookahead with
token* tok;

// Function to match current input token against a token of our choice
bool match_input(int token_type, std::string* name = nullptr, int* value = nullptr);

// Parse Tree visualizer, and as usual with recursive tree visualizers we need an auxiliary function
void visualize_parse_tree(parse_tree_node* root);
void visualize_parse_tree_aux(parse_tree_node* node, std::ostream& out);

// Recursive descent parser implementation, should return the root node of the parse tree
// (the classic (default) backtracking version versus the version optimized with lookahead)
parse_tree_node* recursively_descend();
parse_tree_node* recursively_descend_with_lookahead();

// LL(1) predictive parser implementation, should return the root node of the parse tree
parse_tree_node* ll1_first_follow();

int main(int argc, char const* argv[]) {

    // Read the code file (name passed as an argument to the program from the command line)
    std::cout << "\nNow reading the file: " << argv[1] << std::endl;
    read_file(argv[1], buf);

    // Since I want to demonstrate the C enum statement but the scanner I built was for TINY...
    // Soooo... Let me just extend its reserved keywords list with what I need :'D
    reserved_keywords.push_back("enum");

    // Rules of the grammar that I will test this parser with (the C enum expression from Tut 7):
    // 
    // enum-expr → enum identifier { enum-list } | enum { enum-list }
    // enum-list → enumerator | enumerator, | enumerator, enum-list
    // enumerator → enumeration-constant | enumeration-constant = constant-expression

    parse_tree_node* parse_tree_root_node;

    #if defined(RDB)
    std::cout << "Recursive Descent with backtracking:\n" << std::endl;
    parse_tree_root_node = recursively_descend();

    #elif defined(RDL)
    std::cout << "Recursive Descent with lookahead:\n" << std::endl;
    parse_tree_root_node = recursively_descend_with_lookahead();

    #elif defined(LL1)
    std::cout << "FIRST-FOLLOW style LL(1) predictive parser\n" << std::endl;
    parse_tree_root_node = ll1_first_follow();

    #endif

    std::cout << (parse_tree_root_node ? "Parsing successful.\n" : "Parsing failed.\n") << std::endl;
    if (parse_tree_root_node) visualize_parse_tree(parse_tree_root_node);

    return 0;

}

bool match_input(int token_type, std::string* name, int* value) {

    static int buffer_offset = get_next_token(buf, tok);

    // The comma and curly bracket tokens don't exist in the TINY scanner
    // So, I need to differentiate whether I am checking one of the TINY scanner tokens or not
    // I intentionally made the extra tokens have a value larger than 100, knowing that TINY's enum did not reach 100
    // So if the token type I am evaluating now is 100 or more, it is from the extra tokens
    if (token_type >= COMMA) {

        // For commas, TINY would consider it an ERROR token, thankfully I made my TINY scanner put error tokens content in "name"
        // So if I am looking for a COMMA, I should expect to receive an ERROR token whose "name" is ","
        if (token_type == COMMA && (tok->type != ERROR || tok->name != ",")) return false;
        // For the opening curly bracket, TINY would consider it the beginning of a block comment
        if (token_type == LEFT_CURLY_BRACKET && tok->type != BLOCK_COMMENT) return false;
        // For the closing curly bracket, same idea as with the comma
        if (token_type == RIGHT_CURLY_BRACKET && (tok->type != ERROR || tok->name != "}")) return false;

        // Buuuuut before you leave this block.. remember that you matched the entire BLOCK COMMENT
        // For example, "{ RED, }" would be entirely matched against just the opening curly bracket
        // So I want to return to the input buffer the " RED, }" part to continue consuming from it
        // Thankfully, in my infinite wisdom I decided to put the BLOCK COMMENT in the token's name
        // So, just insert the token's name minus the first character in it (the '{') in the buffer
        if (token_type == LEFT_CURLY_BRACKET) {
            std::cout << "[LOG] | (TINY Workaround) Inserting: " << tok->name.substr(1) << std::endl;
            buf.insert(buffer_offset, tok->name.substr(1));
        }
        // This is some insane woh spaghetti coding right here everyone, don't be like me
        // P.S. This workaround won't work with the regex_iterator scanner implementation

    // If it is not >=100, we're in the normal case, so just do a direct comparison
    } else if (tok->type != token_type) return false;

    // Regarding "name" and "value", your TA decided to pull one more big brain move
    // Since some tokens don't have a name (or you don't care about it), I made it optional; likewise the value
    // How? By making them pointers, such that I can pass a null pointer (the default value in the function)
    // If it is a null pointer, I simply skip checking it
    if (name != nullptr && tok->name != *name) return false;
    if (value != nullptr && tok->value != *value) return false;

    // If all is well, consume the token (i.e., prepare the next token) and return true.
    std::cout << "[LOG] | Token matched in the buffer: " << *tok << std::endl;
    buffer_offset = get_next_token(buf, tok);
    return true;

}

void visualize_parse_tree(parse_tree_node* root) {

    std::ofstream dot_file_out("parse_tree.dot", std::ios::trunc);

    // TODO: Would be nice if you decorated your parse tree, lookup how to do so in graphviz dot notation
    dot_file_out << "digraph G {\n";
    visualize_parse_tree_aux(root, dot_file_out);
    dot_file_out << "}";
    dot_file_out.close();

    // Convert the dot file into a SVG vector image
    system("dot -Tsvg ./parse_tree.dot -oparse_tree.svg");

    // Open the vector file
    // TODO: Integrate this with your GUI...
    // You'll need to lookup how to open an .svg file inside Qt or whatever GUI framework you are using
    system("parse_tree.svg");

}

void visualize_parse_tree_aux(parse_tree_node* node, std::ostream& out) {

    // Need to uniquely identify each node in graphviz
    out << "  " << node << (node->is_terminal ? "[fontcolor=\"red\"][color=\"red\"];\n" : ";\n");

    for (auto child : node->children) {

        bool child_has_children = child->children.size() > 0;
        if (child->is_terminal || (!child->is_terminal && child_has_children)) {

            // Need to uniquely identify each child node too
            out << "  " << child << ";\n";

            // Create edge between node and child unique addresses
            out << "  \"" << (void*) node << "\" -> \"" << (void*) child << "\";\n";

            // Recursively build rest of tree
            visualize_parse_tree_aux(child, out);

        }

    }

}

// ================================================== //
// Recursive Descent with Backtracking Implementation //
// ================================================== //

// P.S. Random C++ notice here:
// You must declare enumexpr before calling it in "recursively_descend" next
// Otherwise, the C++ compiler will complain about seeing you use a function that was not declared so far in the code
// Think about how this can be implemented in a compiler actually while you're at it... symbol table maybe? ¯\_(ツ)_/¯
// So, let me just forward-declare all the functions we will need to represent the grammar right now
// You will notice that I will forward-declare 6 functions for 3 rules...
// ...this is because of left-factoring, as you will see later. Enjoy the ride!
parse_tree_node* enumexpr();
parse_tree_node* enumexprbody();
parse_tree_node* enumlist();
parse_tree_node* enumlistbody();
parse_tree_node* enumlistoptional();
parse_tree_node* enumerator();
parse_tree_node* enumeratorvalueexpr();

// We simply call the top rule, and let the algorithm recursively descend from there
parse_tree_node* recursively_descend() { return enumexpr(); }

// enum-expr → enum identifier { enum-list } | enum { enum-list }
// 
// No can do, top-down parser will complain about this
// Let's do left factoring to fix this problem
// 
// enum-expr      → enum enum-expr-body
// enum-expr-body → identifier { enum-list } 
//                | { enum-list }
// 
// C'est gucci!

// enum-expr      → enum enum-expr-body
parse_tree_node* enumexpr() {

    // Create a new node for this non-terminal
    // This node's name is "enumexpr" and it is a non-terminal (hence pass "false" to the "terminal" flag)
    parse_tree_node* current_node = new parse_tree_node("enum-expr", false);

    // 1. Try to match a reserved "enum" keyword, where if it matches, add it to the node's children
    std::string enum_str = std::string("enum");
    if (match_input(RESERVED_KW, &enum_str)) current_node->children.push_back(new parse_tree_node("enum", true));
    // Otherwise, return a nullptr to signify mismatch (aka syntax error)
    else { return nullptr; }

    // 2. Try to expand the "enum-expr-body" non-terminal into your parse tree -- this is the recursive part!
    parse_tree_node* enumexprbody_nonterminal = enumexprbody();
    // If it expands successfully (indicated by returning a non-null node), add it to the children
    if (enumexprbody_nonterminal) current_node->children.push_back(enumexprbody_nonterminal);
    // Otherwise, return a nullptr
    else { return nullptr; }

    // If all is well, return the node
    return current_node;

}

// enum-expr-body → identifier { enum-list } 
//                | { enum-list }
parse_tree_node* enumexprbody() {

    parse_tree_node* current_node = new parse_tree_node("enum-expr-body", false);

    // In this rule we have two options, and since we have no lookahead we must try both
    // When we try the first option, if it fails, it is ok, we go for the second option
    // Why? Because "identifier" is optional, i.e., if it is not matched, just skip it
    // We can represent it being optional by doing nothing if it does not match
    // If it matches, we consume the identifier anyway, so all good

    // Note that I have to make a copy of the token before calling match_input because...
    // ...match_input() proceeds to the next token when it matches the current token
    // Why do I need to do this? Because I need the name of the token since this is a variable identifier
    token current_token = *tok;
    if (match_input(IDENTIFIER)) current_node->children.push_back(new parse_tree_node("identifier:\\n" + current_token.name, true));
    else { /* do nothing */ }

    if (match_input(LEFT_CURLY_BRACKET)) current_node->children.push_back(new parse_tree_node("{", true));
    else { return nullptr; }

    parse_tree_node* enumlist_nonterminal = enumlist();
    if (enumlist_nonterminal) current_node->children.push_back(enumlist_nonterminal);
    else { return nullptr; }

    if (match_input(RIGHT_CURLY_BRACKET)) current_node->children.push_back(new parse_tree_node("}", true));
    else { return nullptr; }

    return current_node;

}

// enum-list → enumerator 
//           | enumerator, 
//           | enumerator, enum-list
//
// Ne pas gucci! Left factor this rule please
//
// enum-list      → enumerator enum-list-body
// enum-list-body → ε
//                | ,
//                | , enum-list
//
// C'est très gucci!.. or is it? We still have a problem! The common ',' at the left :(
// Ne pas très gucci.. Let's left-factor it
//
// enum-list          → enumerator enum-list-body
// enum-list-body     → ε
//                    | , enum-list-optional
// enum-list-optional → ε
//                    | enum-list
//
// Now it's good, finally

// enum-list → enumerator enum-list-body
parse_tree_node* enumlist() {

    parse_tree_node* current_node = new parse_tree_node("enum-list", false);

    parse_tree_node* enumerator_nonterminal = enumerator();
    if (enumerator_nonterminal) current_node->children.push_back(enumerator_nonterminal);
    else { return nullptr; }

    parse_tree_node* enumlistbody_nonterminal = enumlistbody();
    if (enumlistbody_nonterminal) current_node->children.push_back(enumlistbody_nonterminal);
    else { return nullptr; }

    return current_node;

}

// enum-list-body → ε
//                | , enum-list-optional
parse_tree_node* enumlistbody() {

    parse_tree_node* current_node = new parse_tree_node("enum-list-body", false);

    // In here, if you don't see a comma, just return, but if you do, you may proceed

    if (match_input(COMMA)) current_node->children.push_back(new parse_tree_node(",", true));
    else { return current_node; }

    parse_tree_node* enumlistoptional_nonterminal = enumlistoptional();
    if (enumlistoptional_nonterminal) current_node->children.push_back(enumlistoptional_nonterminal);
    else { return nullptr; }

    return current_node;

}

// enum-list-optional → ε
//                    | enum-list
parse_tree_node* enumlistoptional() {

    parse_tree_node* current_node = new parse_tree_node("enum-list-optional", false);

    parse_tree_node* enumlist_nonterminal = enumlist();
    if (enumlist_nonterminal) current_node->children.push_back(enumlist_nonterminal);
    else { /* do nothing */ }

    return current_node;

}

// enumerator → enumeration-constant 
//            | enumeration-constant = constant-expression
//
// You know what do now
//
// enumerator            → enumeration-constant enumerator-value-expr
// enumerator-value-expr → ε
//                       | = constant-expression

// enumerator            → enumeration-constant enumerator-value-expr
parse_tree_node* enumerator() {

    parse_tree_node* current_node = new parse_tree_node("enumerator", false);

    token current_token = *tok;
    // The "enumeration-constant" follows the identifier pattern
    if (match_input(IDENTIFIER)) current_node->children.push_back(new parse_tree_node("enumeration-constant:\\n" + current_token.name, true));
    else { return nullptr; }

    parse_tree_node* enumeratorvalueexpr_nonterminal = enumeratorvalueexpr();
    if (enumeratorvalueexpr_nonterminal) current_node->children.push_back(enumeratorvalueexpr_nonterminal);
    else { return nullptr; }

    return current_node;

}

// enumerator-value-expr → ε
//                       | = constant-expression
parse_tree_node* enumeratorvalueexpr() {

    parse_tree_node* current_node = new parse_tree_node("enumerator-value-expr", false);

    // If we get an '=', then we should get a constant-expression afterwards
    // If not, then we are in the epsilon branch, so we return immediately
    // Because if we do not return, we would try to parse a NUMBER afterwards and fail and give a false syntax error!
    if (match_input(EQUALS_OP)) current_node->children.push_back(new parse_tree_node("=", true));
    else { return current_node; }

    token current_token = *tok;
    if (match_input(NUMBER)) current_node->children.push_back(new parse_tree_node("constant-expression:\\n" + std::to_string(current_token.value), true));
    else { return nullptr; }

    return current_node;

}

// =============================================== //
// Recursive Descent with Lookahead Implementation //
// =============================================== //

parse_tree_node* enumexpr_with_lookahead();
parse_tree_node* enumexprbody_with_lookahead();
parse_tree_node* enumlist_with_lookahead();
parse_tree_node* enumlistbody_with_lookahead();
parse_tree_node* enumlistoptional_with_lookahead();
parse_tree_node* enumerator_with_lookahead();
parse_tree_node* enumeratorvalueexpr_with_lookahead();

parse_tree_node* recursively_descend_with_lookahead() { return enumexpr_with_lookahead(); }

// enum-expr → enum enum-expr-body
parse_tree_node* enumexpr_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enum-expr", false);

    // Lookahead won't help here since we don't have choices to pick from based on the lookahead anyway

    std::string enum_str = std::string("enum");
    if (match_input(RESERVED_KW, &enum_str)) current_node->children.push_back(new parse_tree_node("enum", true));
    else return nullptr;

    parse_tree_node* enumexprbody = enumexprbody_with_lookahead();
    if (enumexprbody) current_node->children.push_back(enumexprbody);
    else return nullptr;

    return current_node;

}

// enum-expr-body → identifier { enum-list } 
//                | { enum-list }
parse_tree_node* enumexprbody_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enum-expr-body", false);

    // Although we have a choice here, since the choice is whether to omit or keep "identifier", it still doesn't matter

    token current_token = *tok;
    if (match_input(IDENTIFIER)) current_node->children.push_back(new parse_tree_node("identifier:\\n" + current_token.name, true));

    if (match_input(LEFT_CURLY_BRACKET)) current_node->children.push_back(new parse_tree_node("{", true));
    else return nullptr;

    parse_tree_node* enumlist = enumlist_with_lookahead();
    if (enumlist) current_node->children.push_back(enumlist);
    else return nullptr;

    if (match_input(RIGHT_CURLY_BRACKET)) current_node->children.push_back(new parse_tree_node("}", true));
    else return nullptr;

    return current_node;

}

// enum-list → enumerator enum-list-body
parse_tree_node* enumlist_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enum-list", false);

    // No choices here, lookahead useless again

    parse_tree_node* enumerator = enumerator_with_lookahead();
    if (enumerator) current_node->children.push_back(enumerator);
    else return nullptr;

    parse_tree_node* enumlistbody = enumlistbody_with_lookahead();
    if (enumlistbody) current_node->children.push_back(enumlistbody);
    else return nullptr;

    return current_node;

}

// enum-list-body → ε
//                | , enum-list-optional
parse_tree_node* enumlistbody_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enum-list-body", false);

    // Here, we have choices, but only one terminal in the choices (the other is "epsilon")
    // So, the lookahead here can't help much either.. if you don't see a comma, simply return, no problem.

    if (match_input(COMMA)) {
        current_node->children.push_back(new parse_tree_node(",", true));
        parse_tree_node* enumlistoptional = enumlistoptional_with_lookahead();
        if (enumlistoptional) current_node->children.push_back(enumlistoptional);
        else return nullptr;
    }
    return current_node;

}

// enum-list-optional → ε
//                    | enum-list
parse_tree_node* enumlistoptional_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enum-list-optional", false);

    parse_tree_node* enumlist = enumlist_with_lookahead();
    if (enumlist) current_node->children.push_back(enumlist);
    else { /* do nothing */ }

    return current_node;

}

// enumerator → enumeration-constant enumerator-value-expr
parse_tree_node* enumerator_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enumerator", false);

    // No choices here, lookahead useless, AGAIN!

    token current_token = *tok;
    if (match_input(IDENTIFIER)) current_node->children.push_back(new parse_tree_node("enumeration-constant:\\n" + current_token.name, true));
    else return nullptr;

    parse_tree_node* enumeratorvalueexpr = enumeratorvalueexpr_with_lookahead();
    if (enumeratorvalueexpr) current_node->children.push_back(enumeratorvalueexpr);
    else return nullptr;

    return current_node;

}

// enumerator-value-expr → ε
//                       | = constant-expression
parse_tree_node* enumeratorvalueexpr_with_lookahead() {

    parse_tree_node* current_node = new parse_tree_node("enumerator-value-expr", false);

    // Finally, a case that lookahead helps in.. kinda?
    // If we lookahead and see an '=', we proceed with its branch
    // Otherwise, we return the node (since that is the other branch; epsilon "matched")
    if (match_input(EQUALS_OP)) {
        current_node->children.push_back(new parse_tree_node("=", true));
        token current_token = *tok;
        if (match_input(NUMBER)) {
            current_node->children.push_back(new parse_tree_node("constant-expression:\\n" + std::to_string(current_token.value), true));
        } else return nullptr;
    }
    return current_node;

}

// ================================================================ //
// LL(1) Predictive Parser Implemntation with FIRST and FOLLOW sets //
// ================================================================ //

// enum-expr             → enum enum-expr-body
// 
// enum-expr-body        → identifier { enum-list } 
//                       | { enum-list }
// 
// enum-list             → enumerator enum-list-body
// 
// enum-list-body        → ε
//                       | , enum-list-optional
// 
// enum-list-optional    → ε
//                       | enum-list
// 
// enumerator            → enumeration-constant enumerator-value-expr
// 
// enumerator-value-expr → ε
//                       | = constant-expression
// 
// FIRST(enum-expr)             = {"enum"}
// FIRST(enum-expr-body)        = {identifier, '{'}
// FIRST(enum-list)             = {FIRST(enumerator)}
//                              = {enumeration-constant}
// FIRST(enum-list-body)        = {ε, ','}
// FIRST(enum-list-optional)    = {ε, FIRST(enum-list)}
//                              = {ε, FIRST(enumerator)}
//                              = {ε, enumeration-constant}
// FIRST(enumerator)            = {enumeration-constant}
// FIRST(enumerator-value-expr) = {ε, '='}
//
// FOLLOW(enum-expr)             = {$}
// FOLLOW(enum-expr-body)        = {FOLLOW(enum-expr)}
//                               = {$}
// FOLLOW(enum-list)             = {'}', FOLLOW(enum-list-optional)} 
//                               = {'}'}
// FOLLOW(enum-list-body)        = {FOLLOW(enum-list)} 
//                               = {'}', FOLLOW(enum-list-optional)}
//                               = {'}'}
// FOLLOW(enum-list-optional)    = {FOLLOW(enum-list-body)}
//                               = {'}'}                     ** Notice how I dealt with the recursion in the FOLLOW sets here
// FOLLOW(enumerator)            = {FIRST(enum-list-body)}
//                               = {',', FOLLOW(enum-list-body)}
//                               = {',', '}'}
// FOLLOW(enumerator-value-expr) = {FOLLOW(enumerator)}
//                               = {',', '}'}
//
// One way we could implement the "table" is to implement it as a dictionary...
// ...whose key is a terminal/non-terminal pair and value is the associated production as a string vector
// Here I will demonstrate yet another big brain move of how to create a map whose key is a pair
// Thus the map that you see is first indexed with a non-terminal that we currently want to expand,...
// ...then the terminal that we currently have in the input buffer, then the production to be applied
// 
// Here's some theory: remember the two types of issues that we might face with LL(1) FIRST-FOLLOW implementation?
// 1. If a cell is empty, that is a syntax error. In this implementation, it translates to key mismatches!
// 2. If a cell has two productions, that is ambiguity. In this implementation, it translates to key conflicts!
#include <unordered_map>
#include <stack>

parse_tree_node* ll1_first_follow() {

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> production_dictionary;

    // FIRST(enum-expr) = {"enum"}
    // This translates to the rule "enum-expr → enum enum-expr-body"
    production_dictionary["enum-expr"]["enum"] = { "enum", "enum-expr-body" };

    // FIRST(enum-expr-body) = {identifier, '{'}
    production_dictionary["enum-expr-body"]["identifier"] = { "identifier", "{", "enum-list", "}" };
    production_dictionary["enum-expr-body"]["{"] = { "{", "enum-list", "}" };

    // FIRST(enum-list) = {enumeration-constant}
    production_dictionary["enum-list"]["enumeration-constant"] = { "enumerator", "enum-list-body" };

    // FIRST(enum-list-body) = {ε, ','}
    // Since there's an ε in its FIRST set, we need to consider its FOLLOW set as well
    // The FOLLOW set in this case means "if I receive this token, I must end myself, because this token comes after me"
    // FOLLOW(enum-list-body) = {'}'}
    production_dictionary["enum-list-body"][","] = { ",", "enum-list-optional" };
    production_dictionary["enum-list-body"]["}"] = {}; // aka, ε

    // FIRST(enum-list-optional) = {ε, enumeration-constant}
    // FOLLOW(enum-list-optional) = {'}'}
    production_dictionary["enum-list-optional"]["enumeration-constant"] = { "enum-list" };
    production_dictionary["enum-list-optional"]["}"] = {};

    // FIRST(enumerator) = {enumeration-constant}
    production_dictionary["enumerator"]["enumeration-constant"] = { "enumeration-constant", "enumerator-value-expr" };

    // FIRST(enumerator-value-expr) = {ε, '='}
    // FOLLOW(enumerator-value-expr) = {',', '}'}
    production_dictionary["enumerator-value-expr"]["="] = { "=", "constant-expression" };
    production_dictionary["enumerator-value-expr"][","] = {};
    production_dictionary["enumerator-value-expr"]["}"] = {};

    // Now let's create our stack, its items will be the nodes of our parse tree
    std::stack<parse_tree_node*> parsing_stack;
    parsing_stack.push(new parse_tree_node("$", true)); // We always place $ at the bottom of the stack
    parsing_stack.push(new parse_tree_node("enum-expr", false)); // Followed by the top non-terminal of our CFG
    parse_tree_node* root = parsing_stack.top(); // Let's keep track of this top node so we can return it later

    // Prepare the first token to parse
    int buffer_offset = get_next_token(buf, tok);

    // And now let the LL(1) cooking begin
    // While the stack hasn't reached the bottom entry yet, and the buffer still has content
    while (parsing_stack.top()->label != "$" && buffer_offset != 0) {

        // We will be working with the top of stack every iteration, see what to do with it
        parse_tree_node* top_of_stack = parsing_stack.top();

        std::cout << "[LOG] | Current top of stack: \"" << top_of_stack->label;
        std::cout << "\" and current token in buffer: \"" << *tok << "\"\n";

        // If it is a terminal,
        if (top_of_stack->is_terminal) {

            // Check if it matches the current token
            if ((top_of_stack->label == tok->name) ||
                (top_of_stack->label == "identifier" && tok->type == IDENTIFIER) ||
                (top_of_stack->label == "enumeration-constant" && tok->type == IDENTIFIER) ||
                (top_of_stack->label == "constant-expression" && tok->type == NUMBER)) {

                // If it does, pop the stack, get the next token, and move on to the next iteration
                // But before you do, augment the node with the value of the variable (if it is a variable, like identifier or number)
                if (tok->type == IDENTIFIER) parsing_stack.top()->label += ":\\n" + tok->name;
                if (tok->type == NUMBER) parsing_stack.top()->label += ":\\n" + std::to_string(tok->value);
                parsing_stack.pop();
                buffer_offset = get_next_token(buf, tok);
                // Once again the workaround part :) Act as if you don't see it
                if (tok->name.size() && tok->name[0] == '{') {

                    std::cout << "[LOG] | (TINY Workaround) Inserting: " << tok->name.substr(1) << std::endl;
                    buf.insert(buffer_offset, tok->name.substr(1));
                    tok->name = '{';

                }
                continue;

            // Otherwise, parsing failed, return a nullptr for the parse tree
            } else return nullptr;

        // Otherwise, if it is a non-terminal,
        } else {

            // This part is because I am not using a symbol table, so I don't know if the variable that I receive...
            // ...is an "identifier" or an "enumeration-constant", so I manually check which type it is
            // You shouldn't need to do something like this if your implementation is clean
            std::string token;
            if (top_of_stack->label == "enum-expr-body" && tok->type == IDENTIFIER) token = "identifier";
            else if (tok->type == IDENTIFIER) token = "enumeration-constant";
            else if (tok->type == NUMBER) token = "constant-expression";
            else token = tok->name;

            // Check if there exists a valid production from this non-terminal for the current token
            // We do this by checking how many productions exist for this non-terminal for the current token 
            // Since our grammar is LL(1) (i.e., non-ambiguous), there can only be 1 production, or 0 (none)
            if (production_dictionary[top_of_stack->label].count(token) != 0) {

                // If non-zero, first of all pop the stack to remove the current non-terminal that is at the top
                parsing_stack.pop();

                // Then, push its production items into the stack in reverse order (such that leftmost is at the top)
                // Note that if this production produces epsilon, this loop won't run, as size() will be 0
                for (int i = production_dictionary[top_of_stack->label][token].size() - 1; i >= 0; i--) {

                    std::string production_item = production_dictionary[top_of_stack->label][token][i];

                    // Before I proceed, I want to check if this production item is a terminal or a non-terminal
                    // I can do this by trying to find it in the production dictionary non-terminals keys
                    // If it is not found, then it is a terminal; it is not found if "find()" returns the "end()" iterator
                    bool is_terminal = production_dictionary.find(production_item) == production_dictionary.end();

                    // Now I create a node for it
                    parse_tree_node* node = new parse_tree_node(production_item, is_terminal);

                    // Add it to the children of its parent non-terminal
                    // Since I am using a deque, I can push_front instead of push_back
                    // This is to cancel out the backwards iteration of the child nodes
                    top_of_stack->children.push_front(node);

                    // Finally, push it to the stack
                    parsing_stack.push(node);

                }

            // Otherwise, parsing failed, return a nullptr for the parse tree
            } else return nullptr;
        }
    }

    // If the loop finishes with no errors, and the top of the stack is $, with an empty buffer, then all is well
    std::cout << *tok;
    if (parsing_stack.top()->label == "$" && buffer_offset == 0) return root;

    // Otherwise, either the stack or the buffer finished prematurely, so, parse failure
    else return nullptr;

}
