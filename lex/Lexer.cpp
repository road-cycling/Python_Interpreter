#include <iostream>
#include <string>

#include "Lexer.hpp"

Lexer::Lexer(std::ifstream &stream):
    inStream{stream},
    ungottenToken{false},
    tokIdx{0},
    startLine{true},
    _numTabs{0},
    _numSpace{0},
    pythonLexSpace({0})
{}

static inline bool isEqualityOperator(char c) { return c == '=' || c == '!' || c == '<' || c == '>'; };
static inline bool isKeyword(std::string s) {
    return (s == "for" || s == "print" || s == "if" || s == "else" || s == "elif" ||
            s == "def" || s == "and" || s == "or" || s == "while" || s == "not" || s == "in" ||
            s == "range" || s == "len" || s == "def" || s == "return" || s == "append" || s == "pop");
}

inline int Lexer::spacesConsumedOnLine() {
    return _numTabs + _numSpace;
}

std::string Lexer::readEqualityOperator() {

    char c;
    inStream.get(c);

    if ( c == '=' ) {
        inStream.get(c);

        if (c == '>' || c == '<') {
            std::cout << "Invalid RelOP -> " << c << " <- found... exiting. \n";
            exit(1);
        } else if (c == '=') {
            return "==";
        } else {
            inStream.putback(c);
            return "=";
        }
    } else if ( c == '>' ) {
        inStream.get(c);

        if ( c == '=' ) {    //relGTE
            return ">=";
        } else {            //GT
            inStream.putback(c);
            return ">";
        }
    } else if ( c == '<' ) {
        inStream.get(c);

        if (c == '=') { // LTE
            return "<=";
        } else if (c == '>') {//ML NEQ
            return "<>";
        } else { // LT
            inStream.putback(c);
            return "<";
        }
    } else if ( c == '!' ) {
        inStream.get(c);

        if (c == '=') {
            return "!=";
        } else {
            std::cout << "Invalid RelOP -> " << c << " <- found.. exiting. \n";
            exit(1);
        }
    }

    std::cout << "Fatal Error Lexer::readEqualityOperator\n";
    std::cout << "Dump ->" << c << "<-" << std::endl;
    exit(1);

    return "";
}

std::string Lexer::readString() {

    char c;
    inStream.get(c);

    std::string capture = "";
    char escapeOn = '\0';

    if (c == '"')
        escapeOn = '\"';
    if (c == '\'')
        escapeOn = '\'';

    if (escapeOn == '\0') {
        std::cout << "Fatal Error Lexer::readString.. expected {\",'} got ->" << escapeOn << "<-\n";
        exit(1);
    }

    inStream.get(c);
    // keep it simple to continue
    // TODO come back later and deal with escape
    while (c != escapeOn/* && seenEscape == false*/) {
        capture += c;
        inStream.get(c);
    }

    return capture;
}

void Lexer::consumeLine() {

    char c;
    inStream.get(c);

    if (c != '#') {
        std::cout << "Fatal Error Lexer::consumeLine.. expected #, got " << c << "\n";
        exit(1);
    }

    while (c != '\n') {
            if (inStream.eof()) {
            auto tok = std::make_shared<Token>();
            tok->eof() = true;
            _tokens.push(tok);
            return;
        }

        inStream.get(c);
    }

    if ( startLine == false ) {
        auto token = std::make_shared<Token>();
        token->eol() = true;
        _tokens.push(token);
    }

    startLine = true;
    _numTabs = 0;
    _numSpace = 0;
}

//Consume Spaces
//Return True if a newline char eaten with no characters
bool Lexer::consumeLeadingSpaces() {

    if (!startLine) {
        return false;
    }

    char c;
    inStream.get(c);

    while (isspace(c)) {
        if (c == '\t')
            _numTabs += 1;
        else if (c == ' ') {
            _numSpace += 1;
        }
        else if (c == '\n') {
            startLine = true;
            _numTabs = 0;
            _numSpace = 0;
            return true;
        }

        inStream.get(c);
    }

    if (inStream.good())
        inStream.putback(c);

    if ( c == '#' ) {
        consumeLine();
        return true;
    }

    return false;

}

//https://stackoverflow.com/questions/31171075/what-is-the-best-practice-when-passing-a-shared-pointer-to-a-non-owning-function
void Lexer::readNumber(const bool isNegative, const std::shared_ptr<Token>& tok) {
    // int intValue = 0;
    char c;
    std::string number = "";
    bool isFloat = false;
    while( inStream.get(c) && isdigit(c) ) {
        // intValue = intValue * 10 + c - '0';
        number += c;
    }
    if (c == '.') {
        number += c;
        isFloat = true;
        while( inStream.get(c) && isdigit(c) ) {
        // intValue = intValue * 10 + c - '0';
            number += c;
        }
    }
    if (inStream.good())
        inStream.putback(c);

    if ( number == "." ) {
        tok->setKeyword(".");
    }

    else if ( isFloat ) {
        float floatValue = atof(number.c_str());
        if (isNegative)
            floatValue *= -1.;
        tok->setFloat( floatValue );
    }

    else {
        int intValue = stoi(number);
        if (isNegative)
            intValue *= -1;
        tok->setWholeNumber( intValue );
    }


}

std::string Lexer::readName() {

    std::string name;
    char c;

    inStream.get(c);

    if ( c == '_' || isalnum(c) ) {
      name += c;
    } else {
      exit(1);
    }

    while ( inStream.get(c) && ( c == '_' || isalnum(c) || isdigit(c) )) {
      name += c;
    }

    if(inStream.good())  // In the loop, we have read one char too many.
        inStream.putback(c);
    return name;
}


auto Lexer::getToken() -> std::shared_ptr<Token> {

    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    if (!_tokens.empty()) {
        if (debug)
            std::cout << "!_tokens.empty()" << std::endl;
        auto cachedToken = _tokens.front();
        _processedTokens.push_back(cachedToken);
        _tokens.pop();
        return lastToken = cachedToken;

    }

    if (startLine) {
        //Returns true if we consumed the entire line
        // and need to call getToken();
        if (consumeLeadingSpaces()) {
            return getToken();
        }

        // We just consumed leading spaces - which didn't catch a newline
        startLine = false;
        int spacesConsumed = spacesConsumedOnLine();

        int peekStack = pythonLexSpace.top();

        if (spacesConsumed > peekStack) {
            pythonLexSpace.push(spacesConsumed);
            _tokens.emplace(new Token("INDENT"));

        } else if (spacesConsumed < peekStack) {
            // DROP BACK DOWN to new scope @@ smaller
            while (peekStack != spacesConsumed) {
                _tokens.emplace(new Token("DEDENT"));

                if (pythonLexSpace.empty()) {
                    std::cout << "Fatal Error in Lex::getToken()..couldn't parse spaces\n";
                    exit(1);
                }

                pythonLexSpace.pop();
                peekStack = pythonLexSpace.top();
            }

        } else {;;}

        // If we added tokens to our queue we need to return them.
        if (!_tokens.empty()) {
            //Take from queue
            std::shared_ptr<Token> tok = _tokens.front();

            //Save to processed tokens
            _processedTokens.push_back(tok);

            //Pop off stack
            _tokens.pop();
            return lastToken = tok;
        }
    }

    //The above statement consumes leading spaces // tabs.
    //Since those have been consumed we need to worry about spaces between words.
    //These are not recorded

    auto token = std::make_shared<Token>();

    char c;
    inStream.get(c);

    while (c == ' ' && inStream.get(c));

    if (inStream.eof()) {
        token->eof() = true;
    } else if ( isdigit(c) || c == '.') {
        inStream.putback(c);
        readNumber(false, token);
    } else if ( c == '-' ) {
        token->symbol('-');
    } else if ( isEqualityOperator(c) ) {
        inStream.putback(c);
        token->relExp( readEqualityOperator() );
    } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == ',' ||
               c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ':' || c == '[' || c == ']') {
        token->symbol(c);
     }
     else if ( c == '#' ) {
         inStream.putback(c);
         consumeLine();
         return getToken();
     } else if ( c == '\'' || c == '"') {
         inStream.putback(c);
         token->setString( readString() );
     } else if ( isalpha(c) || c == '_' ) {
         inStream.putback(c);

         std::string tokName = readName();
         if (isKeyword(tokName)) {
             token->setKeyword( tokName );
         } else {
             token->setName( tokName );
         }
     } else if ( c == '\n' ) {
         startLine = true;
         _numTabs = 0;
         _numSpace = 0;
         token->eol() = true;

     } else {
         std::cout << "Unknown character in input. ->" << c << "<-\n";
         exit(1);
     }

     _processedTokens.push_back(token);
     return lastToken = token;
}

void Lexer::printProcessedTokens() {
    int i = 0;
    for (auto &&tok : _processedTokens) {
        std::cout << i << ": ";
        tok->print();
        std::cout << std::endl;
        i++;
    }
}

void Lexer::ungetToken() {
    ungottenToken = true;
}
