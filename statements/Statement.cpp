#include <iostream>
#include <iomanip>
#include <optional>
#include <algorithm>

#include "Statement.hpp"

// START "STATEMENT"
Statement::Statement() {}
// END "STATEMENT"
inline std::shared_ptr<TypeDescriptor> copyPtr(TypeDescriptor *ref) {

    if (ref->type() == TypeDescriptor::INTEGER) {
        auto desc = std::make_shared<NumberDescriptor>(TypeDescriptor::INTEGER);
        desc->_value.intValue = dynamic_cast<NumberDescriptor *>(ref)->_value.intValue; //risky

        return desc;

    } else if (ref->type() == TypeDescriptor::DOUBLE) {
        auto desc = std::make_shared<NumberDescriptor>(TypeDescriptor::DOUBLE);
        desc->_value.doubleValue = dynamic_cast<NumberDescriptor *>(ref)->_value.doubleValue;

        return desc;

    } else if (ref->type() == TypeDescriptor::BOOL) {
        auto desc = std::make_shared<NumberDescriptor>(TypeDescriptor::BOOL);
        desc->_value.boolValue = dynamic_cast<NumberDescriptor *>(ref)->_value.boolValue;

        return desc;

    } else if (ref->type() == TypeDescriptor::STRING) {
        auto desc = std::make_shared<StringDescriptor>(TypeDescriptor::STRING);
        desc->_stringValue = dynamic_cast<StringDescriptor *>(ref)->_stringValue;

        return desc;

    } else {
        std::cout << "Error in copyReferencePtr...ref type is " << ref->type() << std::endl;
        exit(1);
        // Silence Warnings
        return nullptr;
    }
}



AssignStmt::AssignStmt(std::string lhsVar, std::unique_ptr<ExprNode> rhsExpr):
    _lhsVariable{lhsVar},
    _rhsExpression{std::move(rhsExpr)}
{}

AssignStmt::~AssignStmt() {
    if (destructor)
        std::cout << "~AssignStmt()" << std::endl;
}


//std::unique_ptr is the C++11 way to express exclusive ownership,
//but one of its most attractive features is that it easily and efficiently converts to a std::shared_ptr.
void AssignStmt::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "void AssignStmt::evaluate(SymTab &symTab)" << std::endl;

    auto rhs = _rhsExpression->evaluate(symTab);

    if (debug) {
        std::cout << "Asssign Evaluate: LHS = " << _lhsVariable << "\t";
        Descriptor::printValue(rhs.get());
        std::cout << std::endl;
    }

    symTab.setValueFor(_lhsVariable, std::move(rhs));
}

void AssignStmt::dumpAST(std::string spaces) {
    std::cout << spaces << "AssignStmt  ";
    std::cout << this << '\t';
    std::cout  << _lhsVariable << " = ";
    _rhsExpression->print();
    std::cout << std::endl;
    _rhsExpression->dumpAST(spaces + '\t');

}
// END "ASSIGNMENTSTATEMENT"

// START "IFSTATEMENT"
IfStatement::IfStatement() :
    _if{nullptr},
    _elif{nullptr},
    _else{nullptr}
{}

IfStatement::~IfStatement() {
    if (destructor)
        std::cout << "~IfStatement()" << std::endl;
}

void IfStatement::addIfStmt(std::unique_ptr<IfStmt> ifStmt) {
    _if = std::move(ifStmt);
}

void IfStatement::addElifStmt(std::unique_ptr<ElifStmt> elifStmt) {
    _elif = std::move(elifStmt);
}

void IfStatement::addElseStmt(std::unique_ptr<ElseStmt> elseStmt) {
    _else = std::move(elseStmt);
}

void IfStatement::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "void IfStatement::evaluate(SymTab &symTab)" << std::endl;

    if ( _if == nullptr ) {
        std::cout << "IfStatement::evaluate(SymTab &symTab) cannot evaluate nullptr _if" << std::endl;
        exit(1);
    }

    if ( _if->evaluate(symTab) )
        return;

    if ( _elif != nullptr && _elif->evaluate(symTab) )
        return;

    if ( _else != nullptr && _else->evaluate(symTab) )
        return;
}

void IfStatement::dumpAST(std::string spaces) {
    std::cout << spaces << "IfStatement  ";
    std::cout << this << '\t';
    std::cout << std::endl;
    if ( _if != nullptr )
        _if->dumpAST(spaces + '\t');
    if ( _elif != nullptr )
        _elif->dumpAST(spaces + '\t');
    if ( _else != nullptr )
        _else->dumpAST(spaces + '\t');

}
// END "IFSTATEMENT"

// START "PRINTSTATEMENT"
PrintStatement::PrintStatement(std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> testList):
    _testList{std::move(testList)}
{}

PrintStatement::~PrintStatement() {
    if (destructor)
        std::cout << "~PrintStatement()" << std::endl;
}

void PrintStatement::evaluate(SymTab &symTab) {
    if (debug)
        std::cout << "void PrintStatement::evaluate(SymTab &symTab)" << std::endl;

    for_each(_testList->begin(), _testList->end(), [&](auto &&item) {
        Descriptor::printValue( item->evaluate(symTab).get() );
        std::cout << " ";
    });
    std::cout << std::endl;
}

void PrintStatement::dumpAST(std::string spaces) {

    std::cout << spaces << "AST_PrintStatement " << this << std::endl;
    for_each(_testList->begin(), _testList->end(), [&](auto &&item) {
        item->dumpAST(spaces + '\t');
    });
}
// END "PRINTSTATEMENT"

//START "RangeStmt"
RangeStmt::RangeStmt(std::string id):
    _id{id}
{}

RangeStmt::~RangeStmt() {
    if (destructor)
        std::cout << "~RangeStmt" << std::endl;
}

void RangeStmt::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "void RangeStmt::Evaluate(SymTab &symTab)" << std::endl;

    parseTestList(symTab);

    if ( symTab.isDefined( _id ) ) {
        std::cout << "Variable " << _id << " is defined - dying (( FIX )) " << std::endl;
        exit(1);
    }

    int start, end, step;

    start = _start.value_or(0);

    //End is required...lets not fall back on the optional
    if ( !_end.has_value() ) {
        std::cout << "No value in range statement for end. FATAL ERROR" << std::endl;
        exit(1);
    }

    end = _end.value(); //THIS IS AN ERR WE MUST HAVE A VALUE FOR END
    step = _step.value_or(1);

    symTab.createEntryFor(_id, start);

    if (start > end && step < 0) {

        for (; Descriptor::Int::getIntValue(symTab.getValueFor(_id)) > end; Descriptor::Int::incrementByN(step, symTab.getValueFor(_id))) {
            _forBody->evaluate(symTab);
        }


    } else if (start < end && 1 <= step) {

        for (; Descriptor::Int::getIntValue(symTab.getValueFor(_id)) < end; Descriptor::Int::incrementByN(step, symTab.getValueFor(_id))) {
            _forBody->evaluate(symTab);
        }

    } else if (start == end) {
        symTab.erase(_id);
        return;
    } else {
        std::cout << "Invalid For Loop" << std::endl;
        std::cout << "Start: " << start << "\t End: " << end << "\t Step: " << step << std::endl;
        exit(1);
    }

    symTab.erase(_id);

}

void RangeStmt::dumpAST(std::string space) {

    std::cout << space;
    std::cout << std::setw(15) << std::left << "AST_RangeStmt " << this;
    std::cout << std::endl;

    _forBody->dumpAST(space + '\t');
}

void RangeStmt::parseTestList(SymTab &symTab) {

    if ( _testList->size() > 3 ) {
        std::cout << "Too many nodes!" << std::endl;
        exit(1);
    }

    int i = 0;
    for_each(_testList->begin(), _testList->end(), [&, this](auto &item) {

        auto desc = item->evaluate(symTab);
        int intVal = Descriptor::Int::getIntValue(desc.get());

        editOptionals(i, std::optional<int>{ intVal });

        i++;

    });

    for (; i < 3; i++)
        editOptionals(i, std::optional<int>{ std::nullopt });

    // Adjust
    if ( !_end.has_value() && !_step.has_value() ) {
        _end = _start;
        _start = std::optional<int>{};
    }
}

// void RangeStmt::addStatements(std::unique_ptr<GroupedStatements> gs) {
//     _forBody = std::move(gs);
// }
void RangeStmt::addStatements(std::unique_ptr<Statements> stmts) {
    _forBody = std::move(stmts);
}

void RangeStmt::addTestList(std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> testList) {
    _testList = std::move(testList);
}

void RangeStmt::editOptionals(int which, std::optional<int> opt) {

    if ( which == 0 )
        _start = opt;

    if ( which == 1 )
        _end = opt;

    if ( which == 2 )
        _step = opt;
}
// END "RangeSTMT"


//START FunctionDefinition
FunctionDefinition::FunctionDefinition(
    std::string funcName,
    std::vector<std::string> paramList,
    std::unique_ptr<FunctionStatements> funcSuite,
    bool hasBeenAddedToSymTab):
    _funcName{funcName},
    _paramList{paramList},
    _funcSuite{std::move(funcSuite)},
    _hasBeenAddedToSymTab{hasBeenAddedToSymTab},
    _hasReturnValue{false}
{}

void FunctionDefinition::evaluate(SymTab &symTab) {
    if ( !_hasBeenAddedToSymTab ) {
        symTab.setFunction(_funcName, std::make_shared<FunctionDefinition>
            (_funcName, _paramList, std::move(_funcSuite), true));
     return;
    }

    _funcSuite->evaluate(symTab);
    //if _funcsuit has a return value, evaluate that
    // and set hasreturn to true and
    // set a node to point to that return valu  e
    // _SUITE_NOT_FUNC_SUITE_FIX->evaluate(symTab);
}

void FunctionDefinition::dumpAST(std::string spaces) {
    std::cout << spaces << "FunctionDef: " << _funcName << " " << this << " ( ";
    for_each(_paramList.begin(), _paramList.end(), [](auto &str) { std::cout << str << " "; });
    std::cout << ")" << std::endl;

    if ( _funcSuite == nullptr ) {
        std::cout << spaces << "Function std::move() reference removed and moved to function map -- cannot dump" << std::endl;
    } else {
        _funcSuite->dumpAST(spaces + '\t');
    }
}


//END FunctionDefinition

// START RETURN STATEMENT
ReturnStatement::ReturnStatement(std::unique_ptr<ExprNode> returnNode):
     _returnNode{std::move(returnNode)}
{}

void ReturnStatement::evaluate(SymTab &symTab) {
    //std::shared_ptr value = (_returnNode->evaluate(symTab)).get();
    auto returnResults = copyPtr(_returnNode->evaluate(symTab).get());
    symTab.setReturnValue(returnResults);
}

void ReturnStatement::dumpAST(std::string spaces) {
    std::cout << spaces << "Return Wrapper: " << this << std::endl;
    _returnNode->dumpAST(spaces + "\t");

}
//END RETURN STATEMENT

//START FUNCTIONCALL
FunctionCallStatement::FunctionCallStatement(std::unique_ptr<ExprNode> exprNodeCall):
    _exprNodeCall{std::move(exprNodeCall)}
{}

void FunctionCallStatement::evaluate(SymTab &symTab) {
    symTab.openScope();
    _exprNodeCall->evaluate(symTab);
    symTab.closeScope();
}

void FunctionCallStatement::dumpAST(std::string spaces) {
    std::cout << spaces << "Function Wrapper: " << this << std::endl;
    _exprNodeCall->dumpAST(spaces + "\t");
}


//END FUNCTIONCALL

// START "STATEMENTS"
Statements::Statements() {}

Statements::~Statements() {
    if (destructor)
        std::cout << "~Statements()" << std::endl;
}

void Statements::addStatement(std::unique_ptr<Statement> statement) {
    _statements.push_back(std::move(statement));
}

void Statements::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "void Statements::evaluate(SymTab &symTab)" << std::endl;

    for (auto &&s: _statements) {
        s->evaluate(symTab);
    }
}

void Statements::dumpAST(std::string spaces) {

    std::cout << spaces << "Stmts  ";
    std::cout << this << '\t';
    std::cout << _statements.size() << std::endl;

    for_each(_statements.begin(), _statements.end(), [&spaces](auto &s) { s->dumpAST(spaces + '\t'); });

}
// END "STATEMENTS"

//START "STATEMENTS"
void FunctionStatements::addStatement(std::unique_ptr<Statement> stmt) {
    _statements.push_back(std::move(stmt));
}

void FunctionStatements::evaluate(SymTab &symTab) {

    for_each(_statements.begin(), _statements.end(), [&](auto &stmt) { stmt->evaluate(symTab); });

    if ( _returnStatement ) {

        _returnStatement->evaluate(symTab);
        //_returnStatement = _returnExpression->evaluate(symTab);
        //_returnValue = _returnExpression->evaluate(symTab);
        // return value getReturnValue();
    }

}

void FunctionStatements::dumpAST(std::string spaces) {
    std::cout << spaces << "FunctionStatements: " << this << std::endl;
    for_each(_statements.begin(), _statements.end(), [&](auto &stmt) { stmt->dumpAST(spaces + "\t"); });
    if ( _returnStatement != nullptr ) {
        _returnStatement->dumpAST(spaces + "\t");
    }
}

void FunctionStatements::setReturnExpression(std::unique_ptr<ExprNode> retVal) {

    std::cout << "helo\n";
    _returnExpression = std::move(retVal);
}

/*std::optional<*/
    std::unique_ptr<TypeDescriptor>
/*>*/ FunctionStatements::getReturnValue() {
    if ( _returnValue )
        return std::move(_returnValue);
    return nullptr;
}


//END "STATEMENTS"


// START "COMPARISON"
Comparison::Comparison() {}
// END "COMPARISON"

IfStmt::IfStmt(
    std::unique_ptr<ExprNode> comp,
    std::unique_ptr<Statements> stmts
) {
    _if.first = std::move(comp);
    _if.second = std::move(stmts);
}

IfStmt::~IfStmt() {
    if (destructor)
        std::cout << "~IfStmt()" << std::endl;
}

bool IfStmt::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "bool IfStmt::evaluate(SymTab &symTab)" << std::endl;

    if ( Descriptor::Bool::getBoolValue( _if.first->evaluate(symTab).get() ) ) {
        _if.second->evaluate(symTab);
        return true;
    }
    return false;
}

void IfStmt::dumpAST(std::string spaces) {
    std::cout << spaces << "IfStmt    ";
    std::cout << this << "\t" << std::endl;
    _if.first->dumpAST(spaces + "\t");
    _if.second->dumpAST(spaces + "\t");

}
// END "IF"


// START "ELIF"
ElifStmt::ElifStmt() {}

ElifStmt::~ElifStmt() {
    if (destructor)
        std::cout << "~ElifStmt()" << std::endl;
}

// void ElifStmt::addStatement(std::unique_ptr<ExprNode> elif, std::unique_ptr<GroupedStatements> stmts) {
void ElifStmt::addStatement(std::unique_ptr<ExprNode> elif, std::unique_ptr<Statements> stmts) {
    _elif.push_back(
        std::pair<
            std::unique_ptr<ExprNode>,
            // std::unique_ptr<GroupedStatements>
            std::unique_ptr<Statements>
        >{
            std::move(elif),
            std::move(stmts)
        }
    );
}


bool ElifStmt::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "bool ElifStmt::evaluate(SymTab &symTab)" << std::endl;

    for ( auto &&item : _elif ) {
        if ( Descriptor::Bool::getBoolValue( item.first->evaluate(symTab).get() ) ) {
            item.second->evaluate(symTab);
            return true;
        }
    }

    return false;
}

void ElifStmt::dumpAST(std::string spaces) {
    std::cout << spaces << "ElifStmt    ";
    std::cout << this << "\t" << std::endl;
    for_each(_elif.begin(), _elif.end(), [&spaces](auto &s) {
        s.first->dumpAST(spaces + "\t");
        s.second->dumpAST(spaces + "\t");
     });
}
// END "ELIF"


// START "ELSE"
// ElseStmt::ElseStmt(std::unique_ptr<GroupedStatements> s):
//     stmts{std::move(s)}
// {}
ElseStmt::ElseStmt(std::unique_ptr<Statements> s):
    _stmts{std::move(s)}
{}

ElseStmt::~ElseStmt() {
    if (destructor)
        std::cout << "~ElseStmt()" << std::endl;
}

bool ElseStmt::evaluate(SymTab &symTab) {

    if (debug)
        std::cout << "bool ElseStmt::evaluate(SymTab &symTab)" << std::endl;

    _stmts->evaluate(symTab);

    return true;
}

void ElseStmt::dumpAST(std::string spaces) {
    std::cout << spaces << "ElseStmt    ";
    std::cout << this << "\t" << std::endl;
    _stmts->dumpAST(spaces + "\t");
}
// END "ELSE"

//START ArrayOperation
ArrayOperation::ArrayOperation(
    std::string id,
    std::string keyword,
    std::unique_ptr<ExprNode> test):
    _id{id},
    _keyword{keyword},
    _test{std::move(test)}
{}

void ArrayOperation::dumpAST(std::string spaces) {
    std::cout << spaces << "ArrayOperation ( " << _keyword << " ): " << _id << " "  << this << std::endl;
    _test->dumpAST(spaces + "\t");
}


void ArrayOperation::evaluate(SymTab &symTab){

    auto arrayRef = symTab.getValueFor( _id );
    auto arrayDescriptor = dynamic_cast<ArrayDescriptor *>(arrayRef);


    if ( _keyword == "append" /*&& arrayDescriptor != nullptr*/ ) {

        auto descriptorLHS = _test->evaluate(symTab);

        if ( descriptorLHS->type() == TypeDescriptor::INTEGER ) {
            arrayDescriptor->addItem(Descriptor::Int::getIntValue(descriptorLHS.get()));
        } else if ( descriptorLHS->type() == TypeDescriptor::DOUBLE ) {
            arrayDescriptor->addItem(Descriptor::Double::getDoubleValue(descriptorLHS.get()));
        } else if ( descriptorLHS->type() == TypeDescriptor::BOOL) {
            arrayDescriptor->addItem(Descriptor::Bool::getBoolValue(descriptorLHS.get()));
        } else if ( descriptorLHS->type() == TypeDescriptor::STRING) {
            arrayDescriptor->addItem(Descriptor::String::getStringValue(descriptorLHS.get()));
        } else {
            std::cout << "Error ArrayOperation::Evaluate" << std::endl;
        }

    // #hotfixinit
        if ( arrayRef->type() == TypeDescriptor::NOTY_ARRAY ) {
            if ( descriptorLHS->type() == TypeDescriptor::INTEGER ) {
                arrayRef->type() = TypeDescriptor::ARRAY_INT;
            } else if ( descriptorLHS->type() == TypeDescriptor::DOUBLE ) {
                arrayRef->type() = TypeDescriptor::ARRAY_DOUBLE;
            } else if ( descriptorLHS->type() == TypeDescriptor::BOOL ) {
                arrayRef->type() = TypeDescriptor::ARRAY_BOOL;
            } else if ( descriptorLHS->type() == TypeDescriptor::STRING ) {
                arrayRef->type() = TypeDescriptor::ARRAY_STRING;
            }
        }
        return;

    }
    
    if ( _keyword == "pop" && arrayDescriptor != nullptr ) {

        int indexTopPop = Descriptor::Int::getIntValue(_test->evaluate(symTab).get());
        arrayDescriptor->pop(indexTopPop);
    }
}
//END ArrayOperation

