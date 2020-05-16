#ifndef EXPRINTER_ARITHEXPR_HPP
#define EXPRINTER_ARITHEXPR_HPP

#include <memory>
#include <vector>
#include <optional>

#include "SymTab.hpp"
#include "Token.hpp"
#include "Descriptor.hpp"
#include "statements/Statement.hpp"


// An ExprNode serves as the base class (super class) for arithmetic expression.
// It forces the derived classes (subclasses) to implement two functions, print and
// evaluate.
class ExprNode {
public:
    ExprNode(std::shared_ptr<Token> token);

    virtual ~ExprNode() = 0;
    const std::shared_ptr<Token>& token();
    virtual void dumpAST(std::string) = 0;
    virtual void print() = 0;
    // virtual TypeDescriptor evaluate(SymTab &) = 0;
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &) = 0;

private:
    std::shared_ptr<Token> _token;
};


// An InfixExprNode is useful to represent binary arithmetic operators.
class InfixExprNode: public ExprNode {  // An expression tree node.

public:
    InfixExprNode(std::shared_ptr<Token> tk);
    ~InfixExprNode();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);

public:
    std::unique_ptr<ExprNode> _left;
    std::unique_ptr<ExprNode> _right;
};

class ComparisonExprNode: public ExprNode {

public:
    ComparisonExprNode(std::shared_ptr<Token> tk);
    ~ComparisonExprNode();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);

public:
    std::unique_ptr<ExprNode> _left;
    std::unique_ptr<ExprNode> _right;
};

class BooleanExprNode: public ExprNode {

public:
    BooleanExprNode(std::shared_ptr<Token> tk);
    ~BooleanExprNode();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);

public:
    std::unique_ptr<ExprNode> _left;
    std::unique_ptr<ExprNode> _right;
};

class WholeNumber: public ExprNode {
public:
    WholeNumber(std::shared_ptr<Token> token);
    ~WholeNumber();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
};

class Double: public ExprNode {
public:
    Double(std::shared_ptr<Token>);
    ~Double();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
};

class Variable: public ExprNode {
public:
    Variable(std::shared_ptr<Token> token);
    ~Variable();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
};

class StringExp: public ExprNode {
public:
    StringExp(std::shared_ptr<Token> token);
    ~StringExp();

    virtual void dumpAST(std::string);
    virtual void print();
    // virtual TypeDescriptor evaluate(SymTab &);
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
};

class FunctionCall: public ExprNode {
public:
    FunctionCall(std::shared_ptr<Token>, std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>>);
    ~FunctionCall() = default;

    virtual void dumpAST(std::string);
    virtual void print();
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
private:
    std::string _functionName;
    std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> _testList;
};


class ArrayInit: public ExprNode {
public:
    ArrayInit(std::shared_ptr<Token>, std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>>);
    ~ArrayInit() = default;

    virtual void dumpAST(std::string);
    virtual void print();

    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);

private:
    std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> _testList;
};

class ArraySubscription: public ExprNode {
public:
    ArraySubscription(std::shared_ptr<Token>, std::string, std::unique_ptr<ExprNode>);
    ~ArraySubscription() = default;

    virtual void dumpAST(std::string);
    virtual void print();

    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
private:
    std::string _id;
    std::unique_ptr<ExprNode> _test;
};

class ArrayLength: public ExprNode {
public:
    ArrayLength(std::shared_ptr<Token>, std::string);
    ~ArrayLength() = default;

    virtual void dumpAST(std::string);
    virtual void print();
    virtual std::unique_ptr<TypeDescriptor> evaluate(SymTab &);
private:
    std::string _id;
};

#endif //EXPRINTER_ARITHEXPR_HPP

/*
wrls-109-75:interpreter_kooshesh Kamm$ for file in *.cpp *.hpp; do
> sed 's/ArithExprNode/ExprNode/g' < $file > foo
> mv foo $file
> done

*/
