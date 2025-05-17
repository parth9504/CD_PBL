#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cctype>

using namespace std;

// Function to check if a string is a valid number (integer or floating-point)
bool isValidNumber(const string &token) {
    if (token.empty() || token == "." || token == "-") return false;

    bool dot_found = false;
    bool digit_found = false;
    size_t start = (token[0] == '-') ? 1 : 0;  //Handle negative numbers

    for (size_t i = start; i < token.size(); ++i) {
        if (token[i] == '.') {
            if (dot_found) return false; // Multiple dots
            dot_found = true;
        } else if (!isdigit(token[i])) {
            return false; // Non-digit character
        } else {
            digit_found = true;
        }
    }
    return digit_found; // Ensure at least one digit
}

// Function to check if a token is a keyword
bool isKeyword(const string &token) {
    static const set<string> keywords = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for", "goto",
        "if", "int", "long", "register", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while", "cout","cin","using","namespace","std","endl"
    };
    return keywords.count(token);
}

// Function to check if a token is a valid identifier
bool isValidIdentifier(const string &token) {
    if (token.empty() || (!isalpha(token[0]) && token[0] != '_')) return false;
    for (size_t i = 1; i < token.size(); ++i) {
        if (!isalnum(token[i]) && token[i] != '_') return false;
    }
    return !isKeyword(token); // Identifiers can't be keywords
}

// Function to check if a token is an operator (math or logical)
bool isOperator(const string &token, vector<string> &math_ops, vector<string> &logical_ops) {
    static const set<string> math_operators = {"+", "-", "*", "/", "=", "%"};
    static const set<string> logical_operators = {">", "<", ">=", "<=", "==", "!=", "&&", "||", "!"};

    if (math_operators.count(token)) {
        math_ops.push_back(token);
        return true;
    }
    if (logical_operators.count(token)) {
        logical_ops.push_back(token);
        return true;
    }
    return false;
}

// Function to check if a character is a delimiter
bool isDelimiter(char ch) {
    static const string delimiters = " \t\n;,(){}[]'\":";
    return delimiters.find(ch) != string::npos;
}

int main() {
    ifstream fin("lexical_analyser_file.txt");
    if (!fin.is_open()) {
        cout << "Error: Could not open file\n";
        return 1;
    }

    vector<string> keywords;      // Keywords
    set<string> identifiers;      // Unique identifiers
    vector<string> math_ops;      // Math operators
    vector<string> logical_ops;   // Logical operators
    vector<string> numbers;       // Numerical values
    vector<string> strings;       // String literals
    vector<string> others;        // Other characters (delimiters)
    vector<string> errors;        // Errors
    string buffer;                // Token accumulator
    int line = 1;                 // Line number tracker
    bool in_single_line_comment = false;
    bool in_multi_line_comment = false;
    bool in_string_literal = false;

    char ch;
    while (fin.get(ch)) {
        // Handle single-line comments
        if (in_single_line_comment) {
            if (ch == '\n') {
                in_single_line_comment = false;
                line++;
            }
            continue;
        }

        // Handle multi-line comments
        if (in_multi_line_comment) {
            if (ch == '*' && fin.peek() == '/') {
                fin.get(); // Consume '/'
                in_multi_line_comment = false;
            } else if (ch == '\n') {
                line++;
            }
            continue;
        }

        // Handle string literals
        if (in_string_literal) {
            buffer += ch;
            if (ch == '"' && buffer[buffer.size() - 2] != '\\') {
                in_string_literal = false; // End of string literal
                strings.push_back(buffer); // Store string literal
                buffer.clear();
            } else if (ch == '\n') {
                errors.push_back("Error at line " + to_string(line) + ": Unterminated string literal");
                in_string_literal = false;
                buffer.clear();
                line++;
            }
            continue;
        }

        // Check for start of comments, preprocessor directives, or string literals
        if (ch == '/' && fin.peek() == '/' && !in_multi_line_comment && !in_string_literal) {
            in_single_line_comment = true;
            fin.get(); // Consume second '/'
            continue;
        }
        if (ch == '/' && fin.peek() == '*' && !in_single_line_comment && !in_string_literal) {
            in_multi_line_comment = true;
            fin.get(); // Consume '*'
            continue;
        }
        if (ch == '#' && !in_single_line_comment && !in_multi_line_comment && !in_string_literal) {
            // Skip preprocessor directive until newline
            while (fin.get(ch) && ch != '\n' && !fin.eof()) {
                // Continue reading until newline
            }
            line++;
            continue;
        }
        if (ch == '"' && !in_single_line_comment && !in_multi_line_comment) {
            in_string_literal = true;
            buffer = ch; // Start with opening quote
            continue;
        }

        // Handle newlines
        if (ch == '\n') {
            line++;
            if (!buffer.empty()) {
                // Process accumulated token
                if (isValidNumber(buffer)) {
                    numbers.push_back(buffer);
                } else if (isKeyword(buffer)) {
                    keywords.push_back(buffer);
                } else if (isValidIdentifier(buffer)) {
                    identifiers.insert(buffer); // Store unique identifier
                } else if (!isOperator(buffer, math_ops, logical_ops)) {
                    errors.push_back("Error at line " + to_string(line) + ": Invalid token '" + buffer + "'");
                }
                buffer.clear();
            }
            continue;
        }

        // Handle multi-character operators
        if ((ch == '>' || ch == '<' || ch == '=' || ch == '!' || ch == '&' || ch == '|') && !in_single_line_comment && !in_multi_line_comment && !in_string_literal) {
            if (!buffer.empty()) {
                // Process accumulated token
                if (isValidNumber(buffer)) {
                    numbers.push_back(buffer);
                } else if (isKeyword(buffer)) {
                    keywords.push_back(buffer);
                } else if (isValidIdentifier(buffer)) {
                    identifiers.insert(buffer); // Store unique identifier
                } else if (!isOperator(buffer, math_ops, logical_ops)) {
                    errors.push_back("Error at line " + to_string(line) + ": Invalid token '" + buffer + "'");
                }
                buffer.clear();
            }
            string op(1, ch);
            if ((ch == '>' || ch == '<' || ch == '=') && fin.peek() == '=') {
                op += fin.get(); // Consume '='
            } else if (ch == '&' && fin.peek() == '&') {
                op += fin.get(); // Consume '&'
            } else if (ch == '|' && fin.peek() == '|') {
                op += fin.get(); // Consume '|'
            } else if (ch == '<' && fin.peek() == '<') {
                op += fin.get(); // Consume '<' for '<<'
            }
            isOperator(op, math_ops, logical_ops);
            continue;
        }

        // Handle single-character operators and delimiters
        if (isOperator(string(1, ch), math_ops, logical_ops) || isDelimiter(ch)) {
            if (!buffer.empty()) {
                // Process accumulated token
                if (isValidNumber(buffer)) {
                    numbers.push_back(buffer);
                } else if (isKeyword(buffer)) {
                    keywords.push_back(buffer);
                } else if (isValidIdentifier(buffer)) {
                    identifiers.insert(buffer); // Store unique identifier
                } else if (!isOperator(buffer, math_ops, logical_ops)) {
                    errors.push_back("Error at line " + to_string(line) + ": Invalid token '" + buffer + "'");
                }
                buffer.clear();
            }
            // Store non-whitespace delimiters
            if (isDelimiter(ch) && !isspace(ch)) {
                others.push_back(string(1, ch));
            }
            continue;
        }

        // Accumulate token
        if (!isspace(ch) && !in_single_line_comment && !in_multi_line_comment && !in_string_literal) {
            buffer += ch;
        }
    }

    // Process any remaining token
    if (!buffer.empty() && !in_string_literal) {
        if (isValidNumber(buffer)) {
            numbers.push_back(buffer);
        } else if (isKeyword(buffer)) {
            keywords.push_back(buffer);
        } else if (isValidIdentifier(buffer)) {
            identifiers.insert(buffer); // Store unique identifier
        } else if (!isOperator(buffer, math_ops, logical_ops)) {
            errors.push_back("Error at line " + to_string(line) + ": Invalid token '" + buffer + "'");
        }
    }

    // Check for unterminated multi-line comment or string literal
    if (in_multi_line_comment) {
        errors.push_back("Error at line " + to_string(line) + ": Unterminated multi-line comment");
    }
    if (in_string_literal) {
        errors.push_back("Error at line " + to_string(line) + ": Unterminated string literal");
    }

    fin.close();

    // Print results
    cout << "Keywords: ";
    for (size_t i = 0; i < keywords.size(); ++i) {
        cout << keywords[i] << (i == keywords.size() - 1 ? "\n" : ", ");
    }

    cout << "Identifiers: ";
    auto it = identifiers.begin();
    for (size_t i = 0; i < identifiers.size(); ++i, ++it) {
        cout << *it << (i == identifiers.size() - 1 ? "\n" : ", ");
    }

    cout << "Math Operators: ";
    for (size_t i = 0; i < math_ops.size(); ++i) {
        cout << math_ops[i] << (i == math_ops.size() - 1 ? "\n" : ", ");
    }

    cout << "Logical Operators: ";
    for (size_t i = 0; i < logical_ops.size(); ++i) {
        cout << logical_ops[i] << (i == logical_ops.size() - 1 ? "\n" : ", ");
    }

    cout << "Numerical Values: ";
    for (size_t i = 0; i < numbers.size(); ++i) {
        cout << numbers[i] << (i == numbers.size() - 1 ? "\n" : ", ");
    }

    cout << "String Literals: ";
    for (size_t i = 0; i < strings.size(); ++i) {
        cout << strings[i] << (i == strings.size() - 1 ? "\n" : ", ");
    }

    cout << "Punctuators/Separators: ";
    for (size_t i = 0; i < others.size(); ++i) {
        cout << others[i] << (i == others.size() - 1 ? "\n" : " ");
    }

    if (!errors.empty()) {
        cout << "Errors:\n";
        for (const auto &error : errors) {
            cout << error << "\n";
        }
    }

    return 0;
}