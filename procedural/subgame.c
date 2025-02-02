#include "subgame.h"
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_SIZE 300

SubGame* create_sub_game(int row, int col) {
    char title[50];
    snprintf(title, 50, "Sous-Tic-Tac-Toe (%d, %d)", row, col);
    
    SubGame* game = malloc(sizeof(SubGame));
    game->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->finished = 0;

    return game;
}

void close_sub_game(SubGame* game, int row, int col, int winner) {
    if (game) {
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        free(game);
    }
}

void subgame_loop(SubGame* game) {
    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        for (int i = 1; i < 3; i++) {
            SDL_RenderDrawLine(game->renderer, i * 100, 0, i * 100, 300);
            SDL_RenderDrawLine(game->renderer, 0, i * 100, 300, i * 100);
        }

        SDL_RenderPresent(game->renderer);
    }

    close_sub_game(game, 0, 0, 0);  // Ferme la fenêtre après fermeture
}
