.SUFFIXES: .o .cpp .x

BUILD_SUBDIRS = statements lex

CFLAGS = -ggdb -std=c++17
objects =  Token.o Parser.o ArithExpr.o SymTab.o lex/Lexer.o statements/Statement.o main.o 

.PHONY: subdirs 

statement.x: $(objects)
	g++ $(CFLAGS) -g -o statement.x $(objects)
	# bash ./tests/tests.sh


subdirs:
	for d in $(BUILD_SUBDIRS); do \
		$(MAKE) -C $$d; \
		done

.cpp.o:
	g++ $(CFLAGS) -g -c $< -o $@


Token.o:  Token.cpp Token.hpp Debug.hpp
ArithExpr.o: ArithExpr.cpp ArithExpr.hpp Token.hpp SymTab.hpp Debug.hpp Descriptor.hpp DescriptorFunctions.hpp
SymTab.o: SymTab.cpp SymTab.hpp Descriptor.hpp Debug.hpp DescriptorFunctions.hpp
Parser.o: Parser.cpp Parser.hpp statements/Statement.hpp Token.hpp SymTab.hpp ArithExpr.hpp Debug.hpp lex/Lexer.hpp
main.o: main.cpp statements/Statement.hpp lex/Lexer.hpp Token.hpp Parser.hpp SymTab.hpp ArithExpr.hpp Debug.hpp DescriptorFunctions.hpp

clean:
	rm -fr *.o *~ *.x
	for d in $(BUILD_SUBDIRS); do \
		$(MAKE) -C $$d clean; \
		done

# 
# all: library.cpp main.cpp
# $@ evaluates to all
# $< evaluates to library.cpp
# $^ evaluates to library.cpp main.cpp
