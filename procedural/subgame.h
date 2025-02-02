#ifndef SUBGAME_H_
#define SUBGAME_H_

#include <SDL2/SDL.h>

typedef struct SubGame {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int board[3][3];
    int finished;
} SubGame;

SubGame* create_sub_game(int row, int col);
void subgame_loop(SubGame* game);
void close_sub_game(SubGame* game, int row, int col, int winner);

#endif  // SUBGAME_H_
