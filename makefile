CC = gcc
OBJS = main.o game.o ILP_solver.o parser.o game_utils.o stack.o turns_list.o solver.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c game.h parser.h solver.h
	$(CC) $(COMP_FLAG) -c $*.c
game.o: game.c game.h
	$(CC) $(COMP_FLAG) -c $*.c
ILP_solver.o: game.o ILP_solver.c ILP_solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
parser.o: game.o parser.c parser.h
	$(CC) $(COMP_FLAG) -c $*.c
game_utils.o: game.o game_utils.c game_utils.h
	$(CC) $(COMP_FLAG) -c $*.c
stack.o: stack.c stack.h
	$(CC) $(COMP_FLAG) -c $*.c
turns_list.o: turns_list.c turns_list.h
	$(CC) $(COMP_FLAG) -c $*.c
solver.o: game.o turns_list.o game_utils.o stack.o ILP_solver.o parser.o solver.c solver.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)