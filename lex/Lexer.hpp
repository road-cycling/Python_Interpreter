#ifndef __LEXER_HPP
#define __LEXER_HPP

#include <fstream>
#include <vector>
#include <memory>
#include <stack>
#include <queue>

#include "../Debug.hpp"
#include "../Token.hpp"

class Lexer {

public:
    Lexer(std::ifstream &inStream);

    bool consumeLeadingSpaces();
    int spacesConsumedOnLine();

    void consumeLine();
    void readNumber(const bool, const std::shared_ptr<Token>&);

    void ungetToken();

    std::string readString();

    std::string readEqualityOperator();
    std::string readName();

    std::shared_ptr<Token> getToken();

    void printProcessedTokens();

private:

    std::shared_ptr<Token> lastToken;
    bool ungottenToken;

    std::stack<int> pythonLexSpace;
    std::queue<std::shared_ptr<Token>> _tokens;
    std::vector<std::shared_ptr<Token>> _processedTokens;
    std::ifstream &inStream;
    int tokIdx;

    bool startLine;
    int _numTabs;
    int _numSpace;

};

#endif