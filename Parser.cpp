#include <iostream>
#include <memory>

#include "Parser.hpp"

Parser::Parser(Lexer &lex):
    lexer{lex}
{}

void Parser::die(std::string where, std::string message, std::shared_ptr<Token> token) {
    std::cout << where << " " << message << std::endl;
    token->print();
    std::cout << std::endl;
    std::cout << "\nThe following is a list of tokens that have been identified up to this point.\n";
    lexer.printProcessedTokens();
    exit(1);
}

std::unique_ptr<Statements> Parser::file_input() {

    std::string scope = "Parser::file_input()";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();
    std::unique_ptr<Statements> stmts = std::make_unique<Statements>();

    while ( !tok->eof() ) {
        lexer.ungetToken();
        stmts->addStatement(stmt());

        tok = lexer.getToken();
    }

    return stmts;
}


std::unique_ptr<Statement> Parser::stmt() {
    // stmt -> simple_stmt | compound_stmt

    std::string scope = "Parser::stmt()";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( tok->isFunc() || tok->isFor() || tok->isIf() ) {

        lexer.ungetToken();
        return compound_stmt();

    } else {
        lexer.ungetToken();
        return simple_stmt();
    }
}


std::unique_ptr<Statement> Parser::simple_stmt() {
    // Parse grammar rule
    // <simple_stmt> -> { print_stmt | assign_stmt | call_stmt | return_stmt } NEWLINE
    // We haven't done -> arrayop

    std::string scope = "Parser::simple_stmt";
    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();


    if ( !(tok->isPrint() || tok->isName() || tok->isReturn()) ) {
        die(scope, "violating rule", tok);
    }

    if (  tok->isPrint() ) {
        lexer.ungetToken();
        std::unique_ptr<PrintStatement> printStmt = print_stmt();
        getEOL(scope);
        return printStmt;

    } 
    else if ( tok->isReturn() ) {
        lexer.ungetToken();
        std::unique_ptr<ReturnStatement> retStmt = return_stmt();

        // return_stmt() eats the EOL
        // getEOL(scope);
        return retStmt;
    }
    else if ( tok->isName() ) {

        auto cachedToken = tok;

        tok = lexer.getToken();
        if ( tok->isAssignmentOperator() ) {
            lexer.ungetToken();
            std::unique_ptr<AssignStmt> assignStmt = assign_stmt(cachedToken);
            getEOL(scope);
            return assignStmt;
        }
        else if (tok->isOpenParen()) {
            lexer.ungetToken();
            std::unique_ptr<FunctionCallStatement> callStmt =
                std::make_unique<FunctionCallStatement> (
                    call(cachedToken)
                );
            getEOL(scope);
            return callStmt;
        } else if ( tok->isPeriod() ) {
            lexer.ungetToken();
            std::unique_ptr<ArrayOperation> arrayop = array_ops(cachedToken);
            getEOL(scope);
            return arrayop;
        } else {
            die(scope, "Unidentified -> 1 <-", tok);
        } // Remember to add else if(tok->isPerioid()) // array operator

    } else {
        die(scope, "Unidentified -> 1 <-", tok);
        return nullptr;
    }

    die(scope, "Error ><><><><", tok);
    return nullptr;
}

void Parser::getEOL(std::string scope) {
    auto tok = lexer.getToken();
    if ( !tok->eol() )
        die(scope, "Generic getEOL", tok);
}


std::unique_ptr<PrintStatement> Parser::print_stmt() {

    std::string scope = "Parser::print_stmt()";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( !tok->isPrint() )
        die(scope, "Expected `PRINT` keyword, instead got", tok);

    auto testList = testlist();

    return std::make_unique<PrintStatement>(std::move(testList));

}

std::unique_ptr<AssignStmt> Parser::assign_stmt(std::shared_ptr<Token> varName) {

    std::string scope = "Parser::assign_stmt";

    if (debug)
        std::cout << scope << std::endl;

    auto assignOp = lexer.getToken();
    if ( !assignOp->isAssignmentOperator() )
        die(scope, "Parser::assign_stmt() expected `ASSIGN_OP` instead got ", assignOp);

    auto tok = lexer.getToken();

    if ( tok->isOpenSquareBracket() ) {
        lexer.ungetToken();
        auto a_init = array_init();
        return std::make_unique<AssignStmt>(varName->getName(), std::move(a_init));
    }

    lexer.ungetToken();

    std::unique_ptr<ExprNode> rightHandSideExpr = test();

    if (debug)
        std::cout << scope << " return" << std::endl;


    return std::make_unique<AssignStmt>(varName->getName(), std::move(rightHandSideExpr));
}

std::unique_ptr<Statement> Parser::compound_stmt() {

    std::string scope = "Parser::compound_stmt";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( tok->isFor() ) {
        lexer.ungetToken();
        return for_stmt();
    } else if ( tok->isIf() ) {
        lexer.ungetToken();
        return if_stmt();
    } else if ( tok->isFunc() ) {
        lexer.ungetToken();
        return func_def();
    }

    die(scope, "Parser::compound_stmt() expected _keyword -> { FOR | IF }, instead got ", tok);
    return nullptr;

}


std::unique_ptr<IfStatement> Parser::if_stmt() {

    // Parses the grammar rule

    // <if_stmt> -> 'if' <test> ':' suite { 'elif' test ':' suite }* ['else' ':' suite]

    std::string scope = "Parser::if_stmt";

    if (debug)
        std::cout << scope << std::endl;

    auto ifStatement = std::make_unique<IfStatement>();
    auto tok = lexer.getToken();

    if (! tok->isIf()) {
        die(scope, "Expected `if` keyword, instead got", tok);
    }

    // ExprNode *comp = test();
    std::unique_ptr<ExprNode> comp = test();

    tok = lexer.getToken();

    if ( !tok->isColon() ) {
        die(scope, "Expected `:` keyword, instead got", tok);
    }

    // std::unique_ptr<GroupedStatements> stmts = suite();
    std::unique_ptr<Statements> stmts = suite();

    auto ifStmt = std::make_unique<IfStmt>(std::move(comp), std::move(stmts));
    ifStatement->addIfStmt(std::move(ifStmt));
    tok = lexer.getToken();

    bool conditionHit = false;

    if (tok->isElIf()) {

        conditionHit = true;
        auto elseIfStatements = std::make_unique<ElifStmt>();

        while (tok ->isElIf()) {

            std::unique_ptr<ExprNode> elifCmp = test();

            tok = lexer.getToken();
            if ( !tok->isColon() ) {
                die(scope, "Expected `:` keyword, instead got", tok);
            }

            // std::unique_ptr<GroupedStatements> stmts = suite();
            std::unique_ptr<Statements> stmts = suite();

            elseIfStatements->addStatement(std::move(elifCmp), std::move(stmts));

            tok = lexer.getToken();
            conditionHit = false;
        }

    //need this unget token for if / elif no else
        // lexer.ungetToken();

        ifStatement->addElifStmt(std::move(elseIfStatements));
    }

    if (tok->isElse()) {
        // std::cout << "Is else" << std::endl;
        conditionHit = true;
        tok = lexer.getToken();
        if ( !tok->isColon() )
            die(scope, "Expected `:` keyword, instead got", tok);

        // std::unique_ptr<GroupedStatements> stmts = suite();
        // std::unique_ptr<Statements> stmts = suite();
        // auto elseStmt = std::make_unique<ElseStmt>(std::move(stmts));
        auto elseStmt = std::make_unique<ElseStmt>(suite());

        ifStatement->addElseStmt(std::move(elseStmt));
    }

    if ( !conditionHit )
        lexer.ungetToken();


    if (debug)
        std::cout << scope << " return" << std::endl;

    return ifStatement;
}

std::unique_ptr<RangeStmt> Parser::for_stmt() {

    std::string scope = "Parser::for_stmt";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if (! tok->isFor() )
        die(scope, "Expected `FOR` _keyword, instead got", tok);

    // GET ID
    tok = lexer.getToken();

    if ( ! tok->isName() )
        die(scope, "Expected `NAME` _keyword, instead got", tok);

    std::string varName = tok->getName();

    tok = lexer.getToken();

    if (! tok->isIn() )
        die(scope, "Expected `IN` _keyword, instead got", tok);


    tok = lexer.getToken();

    if ( !tok->isRange() )
        die(scope, "Expected `RANGE`, instead got", tok);

    tok = lexer.getToken();


    if ( !tok->isOpenParen() )
        die(scope, "Expected `OPENPAREN`, instead got", tok);


    auto list = testlist();

    tok = lexer.getToken();

    if ( !tok->isCloseParen() ) {
        die(scope, "Expected `CLOSEPAREN`, instead got", tok);
    }

    tok = lexer.getToken();

    if ( !tok->isColon() )
        die(scope, "Expected `:` symbol, instead got", tok);

    // std::unique_ptr<GroupedStatements> stmts = suite();
    std::unique_ptr<Statements> stmts = suite();

    if (debug)
        std::cout << scope << " return" << std::endl;

    std::unique_ptr<RangeStmt> range = std::make_unique<RangeStmt>(varName);
    // range->parseTestList(list);
    range->addTestList(std::move(list));
    range->addStatements(std::move(stmts));

    return range;
}

std::unique_ptr<Statement> Parser::func_def() {
    // func_def: 'def' ID '(' [parameter_list] ')' ':' suite
    // Should be func_suite instead of suite
    std::string scope = "Parser::func_def()";

    auto tok = lexer.getToken();

    if ( !tok->isFunc() )
        die(scope, "Expected `def` instead got", tok);

    tok = lexer.getToken();

    if ( !tok->isName() )
        die(scope, "Expected `<ID>` instead got", tok);

    std::string funcName = tok->getName();

    tok = lexer.getToken();

    if ( !tok->isOpenParen() )
        die(scope, "Expected `(` instead got", tok);

    std::vector<std::string> parameterList = parameter_list();

    tok = lexer.getToken();

    if ( !tok->isCloseParen() )
        die(scope, "Expected `)` instead got", tok);

    tok = lexer.getToken();

    if ( !tok->isColon() )
        die(scope, "Expected `:` instead got", tok);
    std::unique_ptr<FunctionStatements> funcSuite = func_suite();

    return std::make_unique<FunctionDefinition>(funcName, parameterList, std::move(funcSuite), false);
}

std::vector<std::string> Parser::parameter_list() {
    // parameter_list: ID {, ID }*
    std::string scope = "Parser::parameter_list()";

    std::vector<std::string> argNames;
    auto tok = lexer.getToken();

    while ( tok->isName() ) {

        if ( tok->isComma() )
            tok = lexer.getToken();

        if ( !tok->isName() )
            die(scope, "Expected `<ID>` got", tok);

        argNames.push_back( tok->getName() );

        tok = lexer.getToken();

        if ( tok->isComma() )
            tok = lexer.getToken();
    }

    lexer.ungetToken();
    return argNames;
}


std::unique_ptr<Statements> Parser::suite() {
    //Parses the grammar rule
    // <suite> -> EOL INDENT stmt+ DEDENT
    std::string scope = "Parser::suite";
    std::unique_ptr<Statements> stmts = std::make_unique<Statements>();

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();  // Expect an EOL
    if ( !tok->eol() )
        die(scope, "Expected an `EOL`, instead got", tok);

    tok = lexer.getToken();      // Expect an INDENT
    if ( !tok->isIndent() )
        die(scope, "Expected an `INDENT`, instead got", tok);

    tok = lexer.getToken();
    while ( !tok->isDedent() ) {
        lexer.ungetToken();
        stmts->addStatement(stmt());
        tok = lexer.getToken();

    }

    if ( tok->isDedent() )
        return stmts;


    die(scope, "Expected Dedent got", tok);
    return nullptr;
}

std::unique_ptr<FunctionStatements> Parser::func_suite() {
    //Parses the grammar rule
    // <func_suite> -> NEWLINE INDENT {stmt | return_stmt} + DEDENT
    std::string scope = "Parser::func_suit()";
    if (debug)
      std::cout << scope << std::endl;

    auto tok = lexer.getToken();  // Parse NEWLINE
    if (!tok->eol())
        die(scope, "Expected an EOL token, instead got: ", tok);

    tok = lexer.getToken();      // Parse INDENT TOKEN
    if (!tok->isIndent())
        die(scope, "Expected an INDENT token, instead got: ", tok );

    std::unique_ptr<FunctionStatements> functionStmts = std::make_unique<FunctionStatements>();
    tok = lexer.getToken();
    while (!tok->isDedent()) {
        if(tok->isReturn()) {
          lexer.ungetToken();
          //Jorge FFS
          functionStmts->setReturnStatement(return_stmt());
        //   stmts->addStatement(return_stmt());
          tok = lexer.getToken();
          break;
        }
        lexer.ungetToken();
        // stmts->addStatement(stmt());
        functionStmts->addStatement(stmt());
        tok = lexer.getToken();
    }

    if ( tok->isDedent())
        return functionStmts;
    die(scope, "Expected a DEDENT token, instead got: ", tok);
    return nullptr;
}


std::unique_ptr<ReturnStatement> Parser::return_stmt() {
    std::string scope = "Parser::return_stmt";
    if (debug)
      std::cout << scope << std::endl;

    auto tok = lexer.getToken();
    if (!tok->isReturn())
        die(scope, "Expected a return TOKEN, instead got: ", tok);

    tok = lexer.getToken();  // Check if return returned nothing
    if (tok->eol())
        return nullptr;
    lexer.ungetToken();

    std::unique_ptr<ReturnStatement> testNode = std::make_unique<ReturnStatement>(test());
    tok = lexer.getToken();
    if(!tok->eol())
        die(scope, "Expected a EOL TOKEN, instead got: ", tok);
    return testNode;

}

std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> Parser::testlist() {

    std::string scope = "Parser::testlist()";

    auto p = std::make_unique<std::vector<std::unique_ptr<ExprNode>>>();

    auto tok = lexer.getToken();

    if ( tok->isCloseParen() ) {
        lexer.ungetToken();
        return p;
    }

    lexer.ungetToken();
    p->push_back(test());

    tok = lexer.getToken();

    while ( tok->isComma() ) {
        p->push_back(test());
        tok = lexer.getToken();
    }

    lexer.ungetToken();
    return p;
}

std::unique_ptr<ExprNode> Parser::test() {

    std::string scope = "*Parser::test()";

    if (debug)
        std::cout << scope << std::endl;

    return or_test();
}

std::unique_ptr<ExprNode> Parser::or_test() {

    std::string scope = "*Parser::or_test()";

    if (debug)
        std::cout << scope << std::endl;

    std::unique_ptr<ExprNode> left = and_test();
    auto tok = lexer.getToken();

    while ( tok->isOr() ) {
        std::unique_ptr<BooleanExprNode> p = std::make_unique<BooleanExprNode>(tok);
        p->_left = std::move(left);
        p->_right = and_test();
        left = std::move(p);
        tok = lexer.getToken();
    }

    lexer.ungetToken();

    if (debug)
        std::cout << scope << " return" << std::endl;

    return left;
}

std::unique_ptr<ExprNode> Parser::and_test() {
    std::string scope = "*Parser::and_test()";

    if (debug)
        std::cout << scope << std::endl;

    std::unique_ptr<ExprNode> left = not_test();
    auto tok = lexer.getToken();

    while ( tok->isAnd() ) {

        std::unique_ptr<BooleanExprNode> p = std::make_unique<BooleanExprNode>(tok);
        p->_left = std::move(left);
        p->_right = not_test();
        left = std::move(p);
        tok = lexer.getToken();
    }
    lexer.ungetToken();


    if (debug)
        std::cout << scope << " return" << std::endl;

    return left;
}

std::unique_ptr<ExprNode> Parser::not_test() {

    std::string scope = "*Parser::not_test()";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( tok->isNot() ) {
        std::unique_ptr<BooleanExprNode> p = std::make_unique<BooleanExprNode>(tok);
        p->_left = not_test();
        return p;

    } else {
        lexer.ungetToken();
        return comparison();
    }
}

std::unique_ptr<ExprNode> Parser::comparison() {
    // This function parses the grammar rule:

    // <comparison> -> <arith_expr> { <comp_op> <arith_expr> }*

    std::string scope = "*Parser::comparison()";

    if (debug)
        std::cout << scope << std::endl;

    std::unique_ptr<ExprNode> left = arith_expr();
    auto tok = lexer.getToken();

    while (tok->isCompOp()) {
        std::unique_ptr<ComparisonExprNode> p = std::make_unique<ComparisonExprNode>(tok);

        p->_left = std::move(left);
        p->_right = arith_expr();
        left = std::move(p);
        tok = lexer.getToken();
    }

    lexer.ungetToken();

    if (debug)
        std::cout << scope << " return" << std::endl;

    return left;
}

std::unique_ptr<ExprNode> Parser::arith_expr() {
    // This function parses the grammar rules:

    // <arith_expr> -> <term> { ( + | - ) <term> }
    std::string scope = "*Parser::arith_expr()";

    if (debug)
        std::cout << scope << std::endl;

    std::unique_ptr<ExprNode> left = term();
    auto tok = lexer.getToken();

    while (tok->isAdditionOperator() || tok->isSubtractionOperator()) {
        std::unique_ptr<InfixExprNode> p = std::make_unique<InfixExprNode>(tok);
        p->_left = std::move(left);
        p->_right = term();
        left = std::move(p);
        tok = lexer.getToken();
    }
    lexer.ungetToken();

    if (debug)
        std::cout << scope << " return" << std::endl;

    return left;
}

//TODO -> add support for -> // <- op in tokenizer
std::unique_ptr<ExprNode> Parser::term() {
    // This function parses the grammar rules:

    // <term> -> <factor> { ( * | / | % | // ) <factor> }*
    std::string scope = "ExprNode *Parser::term()";

    if (debug)
        std::cout << scope << std::endl;

    std::unique_ptr<ExprNode> left = factor();

    auto tok = lexer.getToken();

    while ( tok->isMultiplicationOperator() || tok->isDivisionOperator() || tok->isModuloOperator() ) {
        std::unique_ptr<InfixExprNode> p = std::make_unique<InfixExprNode>(tok);
        p->_left = std::move(left);
        p->_right = factor();
        left = std::move(p);
        tok = lexer.getToken();
    }
    lexer.ungetToken();

    if (debug)
        std::cout << scope << " return" << std::endl;

    return left;
}

std::unique_ptr<ExprNode> Parser::call(std::shared_ptr<Token> ID) {

    std::string scope = "Parser::call()";
    auto tok = lexer.getToken();

    if ( !tok->isOpenParen() ) {
        die(scope, "Expected `(`", tok);
    }

    std::unique_ptr<std::vector<std::unique_ptr<ExprNode>>> tlist = testlist();

    tok = lexer.getToken();

    if ( !tok->isCloseParen() ) {
        die(scope, "Expected `)`", tok);
    }

    return std::make_unique<FunctionCall>(ID, std::move(tlist));

}

std::unique_ptr<ExprNode> Parser::factor() {
    // This function parses the grammar rules:

    // <factor> -> {'-'} <factor> | <atom>
    // TODO: is this correct?
    // fuck me we have 2 lookaheads
    std::string scope = "*Parser::factor()";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( tok->isSubtractionOperator() ) {
        std::unique_ptr<InfixExprNode> p = std::make_unique<InfixExprNode>(tok);
        p->_left = factor();
        p->_right = nullptr;

        return p;

    } else if ( tok->isLen() ) {
        lexer.ungetToken();
        return array_len();
    } else if ( tok->isName() ) {

        auto cachedTok = tok;

        tok = lexer.getToken();

        if ( tok->isOpenSquareBracket() ) {

            lexer.ungetToken();
            return subscription( cachedTok );

        } else if ( tok->isOpenParen() ) {
            lexer.ungetToken();
            return call( cachedTok );
        } else {
            lexer.ungetToken();
            return std::make_unique<Variable>(cachedTok);

        }
    } else {
        lexer.ungetToken();
        return atom();
    }
}


std::unique_ptr<ExprNode> Parser::atom() {
    //This function parses the grammar rules:

    // <atom> -> <id>
    // <atom> -> <number>
    // <atom> -> <string>+
    // <atom> -> '(' <test> ')'
    std::string scope = "Parser::atom";

    if (debug)
        std::cout << scope << std::endl;

    auto tok = lexer.getToken();

    if ( tok->isName() )              // <id>
        return std::make_unique<Variable>(tok);
    else if ( tok->isWholeNumber() )  // <number>
        return std::make_unique<WholeNumber>(tok);
    else if ( tok->isString() )      // <string> NOT +
        return std::make_unique<StringExp>(tok);
    else if ( tok->isFloat() ) /*Swap to isDouble */
        return std::make_unique<Double>(tok);
    else if ( tok->isOpenParen() ) {
        std::unique_ptr<ExprNode> p = test();
        auto token = lexer.getToken();
        if ( !token->isCloseParen() )
            die("Parser::atom", "Expected close-parenthesis, instead got", token);
        return p;
    }
    die(scope, "Unexpected token", tok);

    return nullptr;
}

// std::unique_ptr<ArraySubscription>
std::unique_ptr<ExprNode> Parser::subscription(std::shared_ptr<Token> tk) {
    // subscription -> ID `[` test `]`

    std::string scope = "Parser::subscription()";

    // auto tok = lexer.getToken();

    // if ( !tok->isName() )
    //     die(scope, "Expected `ID` instead got", tok);

    // std::string name = tok->getName();

    auto tok = lexer.getToken();

    if ( !tok->isOpenSquareBracket() )
        die(scope, "Expected `[` instead got", tok);

    auto testNode = test();

    tok = lexer.getToken();

    if ( !tok->isCloseSquareBracket() )
        die(scope, "Expected `]` instead got", tok);

    return std::make_unique<ArraySubscription> (
        std::make_shared<Token>(),
        tk->getName(),
        std::move(testNode)
    );
}

//std::unique_ptr<ArrayInit>
std::unique_ptr<ExprNode> Parser::array_init() {
    // array_init -> `[` std::optional<TESTLIST> `]`

    std::string scope = "Parser::array_init()";

    auto tok = lexer.getToken();

    if ( !tok->isOpenSquareBracket() )
        die(scope, "Expected `[` instead got", tok);

    tok = lexer.getToken();

    if ( tok->isCloseSquareBracket() ) {
        //return empty array init
        return std::make_unique<ArrayInit>(
            std::make_shared<Token>(),
            nullptr 
        );
    } 
    
    lexer.ungetToken();
    auto tl = testlist();

    tok = lexer.getToken();
    if ( !tok->isCloseSquareBracket() )
        die(scope, "Expected `]` instead got", tok);

    return std::make_unique<ArrayInit>(
        std::make_shared<Token>(), //Hooked up LL2
        std::move(tl)
    );

}

//std::unique_ptr<ArrayOperation>
std::unique_ptr<ArrayOperation> Parser::array_ops(std::shared_ptr<Token> tk) {
    // WE NEED TO PASS THIS A TOKEN
    // array_ops -> ID `.`  NREDACT`(` append | pop NREDACT`)` `(` std::opt testlist `)`
    // The function evaluate will check if () has args based on append / pop

    std::string scope = "Parser::array_ops()";

    auto tok = lexer.getToken();

    // if ( !tok->isName() ) { } //
    // passed token no name

    if ( !tok->isPeriod() ) 
        die(scope, "Expected `.` instead got", tok);

    tok = lexer.getToken();

    if ( !(tok->isAppend() || tok->isPop()) )
        die(scope, "Expected (`append` || `pop`) instead got", tok);

    std::string action = tok->getKeyword();

    tok = lexer.getToken();

    if ( !tok->isOpenParen() )
        die(scope, "Expected `(` instead got", tok);

    // tok = lexer.getToken();

    auto test_args = test();

    tok = lexer.getToken();
    if ( !tok->isCloseParen() )
        die(scope, "Expected `)` instead got", tok);

    return std::make_unique<ArrayOperation> (
        //"<ID>", // token will be passed in as this requires 2 lookaheads kooshesh broke rules
        tk->getName(),
        action, //keyword
        std::move(test_args)
    );

}

// std::unique_ptr<ArrayLength>
std::unique_ptr<ExprNode> Parser::array_len() {
    // `len` `(` ID `)`
    
    std::string scope = "Parser::array_len()";

    auto tok = lexer.getToken();

    if ( !tok->isLen() )
        die(scope, "Expected `len` instead got", tok);

    tok = lexer.getToken();

    if ( !tok->isOpenParen() )
        die(scope, "Expected `(` instead got", tok);

    tok = lexer.getToken();

    if ( !tok->isName() )
        die(scope, "Expected `ID` instead got", tok);

    std::string ID = tok->getName();

    tok = lexer.getToken();

    if ( !tok->isCloseParen() )
        die(scope, "Expected `)` instead got", tok);

    return std::make_unique<ArrayLength> (
        std::make_shared<Token>(),
        ID
    );
    // return nullptr
}