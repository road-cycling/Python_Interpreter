#ifndef EXPRINTER_TOKEN_HPP
#define EXPRINTER_TOKEN_HPP

#include <string>

class Token {

public:
    Token();
    Token(std::string tok);

    bool &eof()  { return _eof; }
    bool &eol()  { return _eol; }

    bool eof() const { return _eof; }
    bool eol() const  { return _eol; }

    bool isOpenParen() const  { return _symbol == '('; }
    bool isCloseParen() const { return _symbol == ')'; }

    bool isOpenBracket()  const { return _symbol == '{'; }
    bool isCloseBracket() const { return _symbol == '}'; }

    bool isOpenSquareBracket()  const { return _symbol == '['; }
    bool isCloseSquareBracket() const { return _symbol == ']'; }





    void symbol(char c) { _symbol = c; }
    char symbol() { return _symbol; }

    void relExp(std::string exp) { _relExp = exp; }
    std::string getRelOp() const { return _relExp; }

    bool isRelGT() const     { return _relExp == ">";  } //done
    bool isRelLT() const     { return _relExp == "<";  } //done
    bool isRelGTE() const    { return _relExp == ">="; } //done
    bool isRelLTE() const    { return _relExp == "<="; } //done
    bool isRelEQ() const     { return _relExp == "=="; } //done
    bool isRelNotEQ() const  { return _relExp == "!="; } //done
    bool isRelEQML() const   { return _relExp == "<>"; } //done
    bool isRelAssign() const { return _relExp == "=";  }


    bool isRelOp() const {
      return
        _relExp != "" && (
        isRelGT()     ||
        isRelLT()     ||
        isRelGTE()    ||
        isRelLTE()    ||
        isRelEQ()     ||
        isRelNotEQ()  ||
        isRelEQML()   ||
        isRelAssign()     //needs to be taken out
      );
    }

    bool isCompOp() const { return isRelOp() && !isRelAssign(); }

    bool isColon()     const { return _symbol == ':'; }
    bool isSemiColon() const { return _symbol == ';'; }
    bool isAssignmentOperator() const     { return _relExp == "="; }
    bool isMultiplicationOperator() const { return _symbol == '*'; }
    bool isAdditionOperator() const       { return _symbol == '+'; }
    bool isSubtractionOperator() const    { return _symbol == '-'; }
    bool isModuloOperator() const         { return _symbol == '%'; }
    bool isDivisionOperator() const       { return _symbol == '/'; }
    bool isComma() const                  { return _symbol == ','; }
    bool isArithmeticOperator() const {
        return isMultiplicationOperator() ||
               isAdditionOperator() ||
               isSubtractionOperator() ||
               isModuloOperator() ||
               isDivisionOperator();
    }



    bool isName() const                   { return _name.length() > 0; }
    std::string getName() const                  { return _name; }
    void setName(std::string n) { _name = n; }

    bool isKeyword() const {
      return _keyword.length() > 0;
    }

    std::string getKeyword() const { return _keyword; }

    bool isString() const { return _str.length() > 0; }

    std::string getString() const {
      return _str;
    }

    void setString(std::string str) {
      _str = str;
    }

    void setKeyword(std::string keyword)  {
      _keyword = keyword;
    }

    bool isPrint()  const { return _keyword == "print"; }
    bool isFor()    const { return _keyword == "for";   }
    bool isIf()     const { return _keyword == "if";    }
    bool isElIf()   const { return _keyword == "elif";  }
    bool isElse()   const { return _keyword == "else";  }
    bool isIndent() const { return _keyword == "INDENT";}
    bool isDedent() const { return _keyword == "DEDENT";}
    bool isAnd()    const { return _keyword == "and";   }
    bool isOr()     const { return _keyword == "or";    }
    bool isNot()    const { return _keyword == "not";   }
    bool isIn()     const { return _keyword == "in";    }
    bool isRange()  const { return _keyword == "range"; }
    bool isFunc()   const { return _keyword == "def";   }
    bool isLen()    const { return _keyword == "len";   }
    bool isPeriod() const { return _keyword == ".";     }
    bool isReturn() const { return _keyword == "return";}
    bool isAppend() const { return _keyword == "append";}
    bool isPop()    const { return _keyword == "pop";   }

    bool isFloat()  const  { return _isFloat; }
    float getFloat() const { return _float; }
    void setFloat(float f) {
      _float = f;
      _isFloat = true;
    }

    bool &isWholeNumber() { return _isWholeNumber; }
    bool isWholeNumber() const { return _isWholeNumber; }
    int getWholeNumber() const { return _wholeNumber; }
    void setWholeNumber(int n) {
        _wholeNumber = n;
        isWholeNumber() = true;
    }

    void print() const;

    void dumpData() const;

private:
    std::string _name;
    std::string _relExp;
    std::string _keyword;
    std::string _str;
    bool _eof, _eol;
    bool _isFloat;
    bool _isWholeNumber;
    char _symbol;
    float _float;
    int _wholeNumber;
};

#endif //EXPRINTER_TOKEN_HPP
