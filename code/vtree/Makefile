CC = g++
CFLAGS = -std=c++11 -O3 -Wall -Wextra -Wshadow -g -Wfatal-errors
INC =
LIB =
OBJFILES = preprocessor.o instance.o treewidth.o dtree.o

all: vtree

vtree: main.o $(OBJFILES)
	$(CC) $(INC) $(CFLAGS) main.o $(OBJFILES) -o vtree $(LIB)

%.o: %.cpp
	$(CC) $(INC) $(CFLAGS) -MMD -c $< -o $@

-include *.d

clean:
	rm -f *.o *.d
	rm -f vtree

.PHONY: all clean
