.SUFFIXES: .o .cpp .x 

CFLAGS = -ggdb -std=c++17

.cpp.o:
	echo "Called"
	g++ $(CFLAGS) -g -c $< -o $@
	

Statement.o: Statement.cpp Statement.hpp ../SymTab.hpp ../Debug.hpp 

clean:
	rm -fr *.o *~ *.x