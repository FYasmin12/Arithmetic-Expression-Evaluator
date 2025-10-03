#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <cmath>
#include <algorithm>

using namespace std;

map<string, double> variables;

class ArithmeticEvaluator {
    string expression;
    size_t pos;

    char peek() {
        while (pos < expression.size() && isspace(expression[pos])) {
            pos++;
        }
        return pos < expression.size() ? expression[pos] : '\0';
    }

    char consume() {
        char current = peek();
        pos++;
        return current;
    }

    void throwError(const string &msg) {
        throw runtime_error("Error at position " + to_string(pos) + ": " + msg);
    }

    double parseNumber() {
        size_t start = pos;
        while (isdigit(peek()) || peek() == '.') {
            consume();
        }
        string number = expression.substr(start, pos - start);
        try {
            return stod(number);
        } catch (invalid_argument &) {
            throwError("Invalid number: " + number);
        }
        return 0;
    }

    string parseIdentifier() {
        size_t start = pos;
        while (isalnum(peek()) || peek() == '_') {
            consume();
        }
        return expression.substr(start, pos - start);
    }

    double parsePrimary() {
        char current = peek();
        if (isdigit(current) || current == '.') {
            return parseNumber();
        } else if (isalpha(current)) {
            string varName = parseIdentifier();
            if (variables.find(varName) != variables.end()) {
                return variables[varName];
            } else {
                throwError("Undefined variable: " + varName);
            }
        } else if (current == '(') {
            consume();
            double value = parseExpression();
            if (peek() == ')') {
                consume();
            } else {
                throwError("Unmatched parenthesis");
            }
            return value;
        } else {
            throwError("Unexpected character: " + string(1, current));
        }
        return 0;
    }

    double parseFactor() {
        if (peek() == '-') {
            consume();
            return -parsePrimary();
        } else if (peek() == '+') {
            consume();
        }
        return parsePrimary();
    }

    double parseTerm() {
        double value = parseFactor();
        while (true) {
            char current = peek();
            if (current == '*') {
                consume();
                value *= parseFactor();
            } else if (current == '/') {
                consume();
                double divisor = parseFactor();
                if (divisor == 0) {
                    throwError("Division by zero");
                }
                value /= divisor;
            } else if (current == '%') {
                consume();
                double divisor = parseFactor();
                if (divisor == 0) {
                    throwError("Division by zero in modulus");
                }
                value = fmod(value, divisor);
            } else {
                break;
            }
        }
        return value;
    }

    double parseExpression() {
        double value = parseTerm();
        while (true) {
            char current = peek();
            if (current == '+') {
                consume();
                value += parseTerm();
            } else if (current == '-') {
                consume();
                value -= parseTerm();
            } else {
                break;
            }
        }
        return value;
    }

    void parseAssignment() {
        string varName = parseIdentifier();
        if (peek() == '=') {
            consume();
            double value = parseExpression();
            variables[varName] = value;
            cout << varName << " = " << value << endl;
        } else {
            throwError("Invalid assignment");
        }
    }

    void trimSpaces(string &input) {
        input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());
    }

public:
    double evaluate(string input) {
        trimSpaces(input);
        expression = input;
        pos = 0;

        if (isalpha(peek()) && input.find('=') != string::npos) {
            parseAssignment();
            return 0;
        } else {
            return parseExpression();
        }
    }
};

void printTitle() {
    string title = "=== Arithmetic Expression Evaluator ===";
    int terminalWidth = 80; // Assuming a standard terminal width
    int padding = (terminalWidth - title.size()) / 2;

    cout << string(padding, ' ') << title << endl;
    cout << endl;
}

int main() {
    ArithmeticEvaluator evaluator;
    string input;
    char choice;

    printTitle();
    cout << "1. Evaluate an expression (e.g., 3+5*2)" << endl;
    cout << "2. Assign a variable (e.g., x=10)" << endl;
    cout << "3. Exit" << endl;

    do {
        cout << "\nEnter your choice (1-3): ";
        cin >> choice;

        cin.ignore(); // Clear the input buffer

        switch (choice) {
            case '1': {
                cout << "Enter expression: ";
                getline(cin, input);
                try {
                    double result = evaluator.evaluate(input);
                    cout << "Result: " << result << endl;
                } catch (const exception &e) {
                    cerr << e.what() << endl;
                }
                break;
            }
            case '2': {
                cout << "Enter assignment (e.g., x=10): ";
                getline(cin, input);
                try {
                    evaluator.evaluate(input);
                } catch (const exception &e) {
                    cerr << e.what() << endl;
                }
                break;
            }
            case '3': {
                cout << "Exiting...\n";
                break;
            }
            default: {
                cout << "Invalid choice! Please try again." << endl;
            }
        }
    } while (choice != '3');

    return 0;
}
