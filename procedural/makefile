CC = gcc
CFLAGS = $(shell sdl2-config --cflags) -Wall -Wextra -std=c99 -lm
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf

SRC_TICTACTOE = main.c logic.c rendering.c subgame.c
OBJ_TICTACTOE = $(SRC_TICTACTOE:.c=.o)
EXEC_TICTACTOE = super_tic_tac_toe

SRC_SNAKE = snake.c
OBJ_SNAKE = $(SRC_SNAKE:.c=.o)
EXEC_SNAKE = snake

all: play

$(EXEC_TICTACTOE): $(OBJ_TICTACTOE)
	$(CC) -o $(EXEC_TICTACTOE) $(OBJ_TICTACTOE) $(CFLAGS) $(LDFLAGS)

$(EXEC_SNAKE): $(OBJ_SNAKE)
	$(CC) -o $(EXEC_SNAKE) $(OBJ_SNAKE) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

play: $(EXEC_TICTACTOE) $(EXEC_SNAKE)
	mv $(EXEC_TICTACTOE) play

clean:
	rm -f $(OBJ_TICTACTOE) $(OBJ_SNAKE)

mrproper: clean
	rm -f $(EXEC_TICTACTOE) $(EXEC_SNAKE) play