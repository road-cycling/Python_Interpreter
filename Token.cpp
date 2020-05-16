#include <iostream>
#include <iomanip>

#include "Token.hpp"
#include "Debug.hpp"

Token::Token():
  _name{""},
  _relExp{""},
  _keyword{""},
  _eof{false},
  _eol{false},
  _isFloat{false},
  _isWholeNumber{false},
  _symbol{'\0'},
  _wholeNumber{0},
  _float{0.}
  {}

Token::Token(std::string tok) {
  _keyword = tok;
}

void Token::dumpData() const {
  std::cout << "_name: " << _name << std::endl;
  std::cout << "_relExp: " << _relExp << std::endl;
  std::cout << "_keyword: " << _keyword << std::endl;
  std::cout << "_eof: " << _eof << std::endl;
  std::cout << "_eol: " << _eol << std::endl;
  std::cout << "_isWholeNumber: " << _isWholeNumber << std::endl;
  std::cout << "_symbol: " << _symbol << std::endl;
  std::cout << "_wholeNumber: " << _wholeNumber << std::endl;
}

void Token::print() const {
    if (tokenDebug)
      std::cout << std::setw(15) << std::left;
    if( eol() )                             std::cout << "EOL" ;
    else if ( eof() )                       std::cout << "EOF" ;
    else if ( isOpenParen() )               std::cout << "(" ;
    else if ( isCloseParen() )              std::cout << ")" ;
    else if ( isOpenSquareBracket() )       std::cout << "[";
    else if ( isCloseSquareBracket() )      std::cout << "]";
    else if ( isAssignmentOperator() )      std::cout << "=" ;
    else if ( isSemiColon() )               std::cout << ";" ;
    else if ( isColon() )                   std::cout << ":" ;
    else if ( isMultiplicationOperator() )  std::cout << "*" ;
    else if ( isAdditionOperator() )        std::cout << "+" ;
    else if ( isSubtractionOperator() )     std::cout << "-" ;
    else if ( isModuloOperator() )          std::cout << "%" ;
    else if ( isDivisionOperator() )        std::cout << "/" ;
    else if ( isReturn())                   std::cout << "return";
    else if ( isName() )                    std::cout << getName();
    else if ( isWholeNumber() )             std::cout << getWholeNumber();
    else if ( isFloat() )                   std::cout << getFloat();
    else if ( isRelOp() )                   std::cout << getRelOp();
    else if ( isOpenBracket() )             std::cout << "{";
    else if ( isCloseBracket() )            std::cout << "}";
    else if ( isComma() )                   std::cout << ",";
    else if ( isKeyword() ) {
      if ( isPrint() )       std::cout << "print";
      else if ( isFor() )    std::cout << "for";
      else if ( isIf() )     std::cout << "if";
      else if ( isElIf() )   std::cout << "elif";
      else if ( isElse() )   std::cout << "else";
      else if ( isIndent() ) std::cout << "INDENT";
      else if ( isDedent() ) std::cout << "DEDENT";
      else if ( isAnd() )    std::cout << "and";
      else if ( isOr() )     std::cout << "or";
      else if ( isNot() )    std::cout << "not";
      else if ( isIn() )     std::cout << "in";
      else if ( isRange() )  std::cout << "range";
      else if ( isPeriod() ) std::cout << ".";
      else if ( isFunc() )   std::cout << "def";
      else if ( isLen() )    std::cout << "len";
      else {
        std::cout << "Unidentified Keyword ... update Token.cpp / Token.hpp";
      }
    }
    else if ( isString() ) {
      std::cout << getString();
    } else {
      std::cout << "Uninitialized token.\n";
      dumpData();
    }

}
