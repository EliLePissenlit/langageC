#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 3
#define WINDOW_SIZE 300

int main_board[BOARD_SIZE][BOARD_SIZE] = {0};

void handle_main_board_click(int x, int y, SDL_Renderer* main_renderer) {
    int row = y / (WINDOW_SIZE / BOARD_SIZE);
    int col = x / (WINDOW_SIZE / BOARD_SIZE);

    if (main_board[row][col] == 0) {
        int game_result = system("./snake");  // Lance Snake et récupère son résultat

        if (game_result == 0) {
            printf("Snake fermé manuellement.\n");
        } else if (game_result == -1) {
            printf("Défaite au Snake. Case laissée vide.\n");
        } else if (game_result == 1) {
            printf("Victoire au Snake ! Marquage de la case.\n");
            main_board[row][col] = 1;  // Marquer la victoire dans la grille
            
            // Rafraîchir immédiatement l'affichage après modification
            SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
            SDL_RenderClear(main_renderer);
            SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);

            for (int i = 1; i < BOARD_SIZE; i++) {
                int step = WINDOW_SIZE / BOARD_SIZE;
                SDL_RenderDrawLine(main_renderer, i * step, 0, i * step, WINDOW_SIZE);
                SDL_RenderDrawLine(main_renderer, 0, i * step, WINDOW_SIZE, i * step);
            }

            // Dessiner les croix rouges (X)
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    if (main_board[r][c] == 1) { // Joueur 1 (X)
                        SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255); // Rouge vif
                        int center_x = c * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));
                        int center_y = r * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));
                        int size = WINDOW_SIZE / (3 * BOARD_SIZE);
                        SDL_RenderDrawLine(main_renderer, center_x - size, center_y - size, center_x + size, center_y + size);
                        SDL_RenderDrawLine(main_renderer, center_x + size, center_y - size, center_x - size, center_y + size);
                    }
                }
            }

            // Mettre à jour l'affichage
            SDL_RenderPresent(main_renderer);
        }
    }
}

void main_game_loop(SDL_Window* main_window, SDL_Renderer* main_renderer) {
    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                handle_main_board_click(e.button.x, e.button.y, main_renderer);
            }
        }

        // Nettoyer l'écran
        SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
        SDL_RenderClear(main_renderer);

        // Dessiner la grille
        SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
        for (int i = 1; i < BOARD_SIZE; i++) {
            int step = WINDOW_SIZE / BOARD_SIZE;
            SDL_RenderDrawLine(main_renderer, i * step, 0, i * step, WINDOW_SIZE);
            SDL_RenderDrawLine(main_renderer, 0, i * step, WINDOW_SIZE, i * step);
        }

        // Dessiner les croix rouges (X)
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (main_board[row][col] == 1) { // Joueur 1 (X)
                    SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255); // Rouge vif
                    int center_x = col * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));
                    int center_y = row * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));
                    int size = WINDOW_SIZE / (3 * BOARD_SIZE);
                    SDL_RenderDrawLine(main_renderer, center_x - size, center_y - size, center_x + size, center_y + size);
                    SDL_RenderDrawLine(main_renderer, center_x + size, center_y - size, center_x - size, center_y + size);
                }
            }
        }

        SDL_RenderPresent(main_renderer);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* main_window = SDL_CreateWindow("Super Tic-Tac-Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer* main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    
    main_game_loop(main_window, main_renderer);
    
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    
    return 0;
}
