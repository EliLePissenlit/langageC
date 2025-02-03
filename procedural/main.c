#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>  // Pour récupérer le statut de sortie
#define M_PI 3.14159265358979323846

#define BOARD_SIZE 3
#define WINDOW_SIZE 300

int main_board[BOARD_SIZE][BOARD_SIZE] = {0};
int game_over = 0;  // Indique si la partie est terminée
TTF_Font *font = NULL;  // Police pour le texte

void restart_game() {
    game_over = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            main_board[i][j] = 0;
        }
    }
}

void render_end_screen(SDL_Renderer* main_renderer, const char* message) {
    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
    SDL_RenderClear(main_renderer);

    // Afficher le message de victoire
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(main_renderer, text_surface);
    int text_width = text_surface->w;
    int text_height = text_surface->h;
    SDL_Rect text_rect = {
        (WINDOW_SIZE - text_width) / 2,
        (WINDOW_SIZE - text_height) / 2 - 20,
        text_width,
        text_height
    };

    SDL_RenderCopy(main_renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    // --- Calculer la taille du bouton "Rejouer" ---
    int tw, th;
    TTF_SizeText(font, "Rejouer", &tw, &th);
    int margin_x = 20, margin_y = 10;
    int button_w = tw + margin_x;
    int button_h = th + margin_y;
    int button_x = (WINDOW_SIZE - button_w) / 2;
    int button_y = WINDOW_SIZE - 60;

    SDL_SetRenderDrawColor(main_renderer, 200, 200, 200, 255);
    SDL_Rect button_rect = {button_x, button_y, button_w, button_h};
    SDL_RenderFillRect(main_renderer, &button_rect);

    // Centrer le texte "Rejouer" dans le bouton
    int text_x = button_x + (button_w - tw) / 2;
    int text_y = button_y + (button_h - th) / 2;
    SDL_Surface* button_surface = TTF_RenderText_Solid(font, "Rejouer", (SDL_Color){0, 0, 0, 255});
    SDL_Texture* button_texture = SDL_CreateTextureFromSurface(main_renderer, button_surface);
    SDL_Rect button_text_rect = {text_x, text_y, tw, th};
    SDL_RenderCopy(main_renderer, button_texture, NULL, &button_text_rect);

    SDL_FreeSurface(button_surface);
    SDL_DestroyTexture(button_texture);

    // Affichage final
    SDL_RenderPresent(main_renderer);
}


void render_game(SDL_Renderer* main_renderer, const char* message) {
    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
    SDL_RenderClear(main_renderer);

    SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
    for (int i = 1; i < BOARD_SIZE; i++) {
        int step = WINDOW_SIZE / BOARD_SIZE;
        SDL_RenderDrawLine(main_renderer, i * step, 0, i * step, WINDOW_SIZE);
        SDL_RenderDrawLine(main_renderer, 0, i * step, WINDOW_SIZE, i * step);
    }

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int center_x = col * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));
            int center_y = row * (WINDOW_SIZE / BOARD_SIZE) + (WINDOW_SIZE / (2 * BOARD_SIZE));

            if (main_board[row][col] == 1) { // Croix rouge
                SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
                int size = WINDOW_SIZE / (3 * BOARD_SIZE);
                SDL_RenderDrawLine(main_renderer, center_x - size, center_y - size, center_x + size, center_y + size);
                SDL_RenderDrawLine(main_renderer, center_x + size, center_y - size, center_x - size, center_y + size);
            } else if (main_board[row][col] == 2) { // Cercle bleu
                SDL_SetRenderDrawColor(main_renderer, 0, 0, 255, 255);
                int radius = WINDOW_SIZE / (3 * BOARD_SIZE);
                for (int angle = 0; angle < 360; angle++) {
                    float rad = angle * (M_PI / 180);
                    int x1 = center_x + radius * cos(rad);
                    int y1 = center_y + radius * sin(rad);
                    SDL_RenderDrawPoint(main_renderer, x1, y1);
                }
            }
        }
    }

    SDL_RenderPresent(main_renderer);
}

int check_winner() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (main_board[i][0] != 0 && main_board[i][0] == main_board[i][1] && main_board[i][1] == main_board[i][2]) {
            return main_board[i][0];
        }
        if (main_board[0][i] != 0 && main_board[0][i] == main_board[1][i] && main_board[1][i] == main_board[2][i]) {
            return main_board[0][i];
        }
    }
    if (main_board[0][0] != 0 && main_board[0][0] == main_board[1][1] && main_board[1][1] == main_board[2][2]) {
        return main_board[0][0];
    }
    if (main_board[0][2] != 0 && main_board[0][2] == main_board[1][1] && main_board[1][1] == main_board[2][0]) {
        return main_board[0][2];
    }
    return 0;
}

void display_winner(SDL_Renderer* main_renderer, int winner) {
    game_over = 1;
    if (winner == 1) {
        render_end_screen(main_renderer, "Le joueur a gagne !");
    } else if (winner == 2) {
        render_end_screen(main_renderer, "Le bot a gagne !");
    }
}

void main_game_loop(SDL_Renderer* main_renderer) {
    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (game_over) {
                    if (x > WINDOW_SIZE / 4 && x < 3 * WINDOW_SIZE / 4 &&
                        y > WINDOW_SIZE - 60 && y < WINDOW_SIZE - 20) {
                        restart_game();
                        render_game(main_renderer, NULL);
                    }
                    continue;
                }

                int row = y / (WINDOW_SIZE / BOARD_SIZE);
                int col = x / (WINDOW_SIZE / BOARD_SIZE);

                if (main_board[row][col] != 0) continue;

                int status = system("./snake");
                int game_result = WEXITSTATUS(status);

                if (game_result == 1) {
                    main_board[row][col] = 1;
                    render_game(main_renderer, NULL);

                    int winner = check_winner();
                    if (winner) {
                        display_winner(main_renderer, winner);
                        continue;
                    }
                }

                int bot_row = rand() % BOARD_SIZE;
                int bot_col = rand() % BOARD_SIZE;
                while (main_board[bot_row][bot_col] != 0) {
                    bot_row = rand() % BOARD_SIZE;
                    bot_col = rand() % BOARD_SIZE;
                }
                main_board[bot_row][bot_col] = 2;
                render_game(main_renderer, NULL);

                int winner = check_winner();
                if (winner) display_winner(main_renderer, winner);
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    font = TTF_OpenFont("DejaVuSans.ttf", 32);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Window* main_window = SDL_CreateWindow("Super Tic-Tac-Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer* main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);

    render_game(main_renderer, NULL);
    main_game_loop(main_renderer);

    return 0;
}
