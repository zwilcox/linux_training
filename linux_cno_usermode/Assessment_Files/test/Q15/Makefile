CC=gcc

CC_ARGS=-g -Werror -Wall -Wextra

all: cno_poke cno_poke_handler_solution
	
cno_poke: cno_poke.c
	$(CC) $(CC_ARGS) -o $@ $^
	strip --keep-symbol=changeme $@

cno_poke_handler_solution: cno_poke_handler_answer.c
	$(CC) $(CC_ARGS) -o $@ $^

clean:
	rm -f cno_poke
	rm -f cno_poke_handler_solution

