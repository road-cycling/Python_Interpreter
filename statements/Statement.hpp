#ifndef __STATEMENT_HPP
#define __STATEMENT_HPP

#include <memory>
#include <vector>
#include <optional>
#include "../SymTab.hpp"
#include "../ArithExpr.hpp"
#include "../Token.hpp"
#include "../Debug.hpp"
#include "../Descriptor.hpp"
#include "../DescriptorFunctions.hpp"
class IfStmt;
class ElifStmt;
class ElseStmt;
class ExprNode;
class ReturnStatement;
//class FunctionDefinition;

class Statement {

public:
    Statement();

    virtual ~Statement() = default;
    virtual void evaluate(SymTab &symTab) = 0;
    virtual void dumpAST(std::string) = 0;
};


class Statements {

public:
    Statements();

    void addStatement(std::unique_ptr<Statement> statement);
    void evaluate(SymTab &symTab);
    ~Statements();

    void dumpAST(std::string);

    int length() { return _statements.size(); }

public:
    std::vector<std::unique_ptr<Statement>> _statements;
};

class FunctionStatements {
public:
    FunctionStatements() = default;
    ~FunctionStatements() = default;

    void addStatement(std::unique_ptr<Statement>);
    void evaluate(SymTab &symTab);
    void dumpAST(std::string);

    void setReturnExpression(std::unique_ptr<ExprNode>);
    /*std::optional<
        std::unique_ptr<TypeDescriptor>
    >*/std::unique_ptr<TypeDescriptor> getReturnValue();

    void setReturnStatement(std::unique_ptr<ReturnStatement> returnStmt) {
        _returnStatement = std::move(returnStmt);
    }

private:
    std::vector<std::unique_ptr<Statement>> _statements;
    std::unique_ptr<ReturnStatement> _returnStatement; //?
    std::unique_ptr<ExprNode> _returnExpression{nullptr};
    std::unique_ptr<TypeDescriptor> _returnValue{nullptr};

};


class AssignStmt : public Statement {

public:
    // AssignStmt(std::string, ExprNode *);
    AssignStmt(std::string, std::unique_ptr<ExprNode>);

    virtual ~AssignStmt();
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);
private:
    std::string _lhsVariable;
    std::unique_ptr<ExprNode> _rhsExpression;
    // ExprNode *_rhsExpression;
};

class IfStatement : public Statement {

public:
    IfStatement();

    void addIfStmt(std::unique_ptr<IfStmt>);
    void addElifStmt(std::unique_ptr<ElifStmt>);
    void addElseStmt(std::unique_ptr<ElseStmt>);

    virtual ~IfStatement();
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

private:
    std::unique_ptr<IfStmt>   _if;
    std::unique_ptr<ElifStmt> _elif;
    std::unique_ptr<ElseStmt> _else;
};

class PrintStatement : public Statement {

public:
    // PrintStatement(std::string);
    PrintStatement(std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>>);

    virtual ~PrintStatement();
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

private:
    std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> _testList;
};


class RangeStmt : public Statement {

public:
    RangeStmt(std::string);

    virtual ~RangeStmt();
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

    void parseTestList(SymTab &symTab);

    // void addStatements(std::unique_ptr<GroupedStatements>);
    void addStatements(std::unique_ptr<Statements>);

    void addTestList(std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>>);
    void editOptionals(int, std::optional<int>);

private:
    std::string _id;
    std::optional<int> _start, _end, _step;

    // std::unique_ptr<GroupedStatements> _forBody;
    std::unique_ptr<Statements> _forBody;

    std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> _testList;
};

class FunctionDefinition : public Statement {
public:
    FunctionDefinition(std::string, std::vector<std::string>, std::unique_ptr<FunctionStatements>, bool);
    virtual ~FunctionDefinition() = default;
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);


    std::vector<std::string>& getParamList() { return _paramList; }
    int paramSize() { return _paramList.size();}
    FunctionStatements* getStatements() { return _funcSuite.get(); }

private:

    std::unique_ptr<FunctionStatements> _funcSuite;
    std::vector<std::string> _paramList;
    std::string _funcName;
    bool _hasBeenAddedToSymTab;
    bool _hasReturnValue;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(std::unique_ptr<ExprNode>);
    virtual ~ReturnStatement() = default;
    virtual void evaluate(SymTab &symTab);
    //have a function that returns returnnode evaluationg
    //std::unique_ptr<ExprNode> returnNode()
    virtual void dumpAST(std::string);
private:
  std::unique_ptr<ExprNode> _returnNode;

};

class FunctionCallStatement : public Statement {
public:
    FunctionCallStatement(std::unique_ptr<ExprNode>);
    virtual ~FunctionCallStatement() = default;
    virtual void evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);
private:
    std::unique_ptr<ExprNode> _exprNodeCall;
};

class ArrayOperation: public Statement {
public:
    ArrayOperation(std::string, std::string, std::unique_ptr<ExprNode>);
    ~ArrayOperation() = default;

    virtual void dumpAST(std::string);

    virtual void evaluate(SymTab &);

private:
    std::string _id; // <ID>
    std::string _keyword; // append || pop
    std::unique_ptr<ExprNode> _test;
};

class Comparison {

public:
    Comparison();
    virtual ~Comparison() = default;
    virtual bool evaluate(SymTab &symTab) = 0;
    virtual void dumpAST(std::string) = 0;
};

class IfStmt : public Comparison {

public:

    // IfStmt(
    //     std::unique_ptr<ExprNode>,
    //     std::unique_ptr<GroupedStatements>
    // );

    IfStmt(
        std::unique_ptr<ExprNode>,
        std::unique_ptr<Statements>
    );

    virtual ~IfStmt(); /* = default;*/
    virtual bool evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

private:
    std::pair<
        std::unique_ptr<ExprNode>,
        // std::unique_ptr<GroupedStatements>
        std::unique_ptr<Statements>
    > _if;
};

class ElifStmt : public Comparison {

public:
    ElifStmt();
    virtual ~ElifStmt(); /* = default*/;

    // void addStatement(std::unique_ptr<ExprNode>, std::unique_ptr<GroupedStatements>);
    // void addStatement(ExprNode *, std::unique_ptr<GroupedStatements>);
    void addStatement(std::unique_ptr<ExprNode>, std::unique_ptr<Statements>);

    virtual bool evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

private:
    std::vector<
        std::pair<
            std::unique_ptr<ExprNode>,
            // std::unique_ptr<GroupedStatements>
            std::unique_ptr<Statements>
        >
    > _elif;
};

class ElseStmt : public Comparison {

public:
    // ElseStmt(std::unique_ptr<GroupedStatements>);
    ElseStmt(std::unique_ptr<Statements>);
    virtual ~ElseStmt(); /* = default;*/

    virtual bool evaluate(SymTab &symTab);
    virtual void dumpAST(std::string);

private:
    // std::unique_ptr<GroupedStatements> stmts;
    std::unique_ptr<Statements> _stmts;
};


#endif
