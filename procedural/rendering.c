#include "rendering.h"

void render_board(SDL_Renderer* renderer, int board[3][3]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 1; i < 3; i++) {
        SDL_RenderDrawLine(renderer, i * 100, 0, i * 100, 300);
        SDL_RenderDrawLine(renderer, 0, i * 100, 300, i * 100);
    }

    SDL_RenderPresent(renderer);
}
