.SUFFIXES: .o .cpp .x 

CFLAGS = -ggdb -std=c++17

.cpp.o:
	g++ $(CFLAGS) -g -c $< -o $@
	
Lexer.o: Lexer.cpp Lexer.hpp ../SymTab.hpp ../Debug.hpp 

clean:
	rm -fr *.o *~ *.x