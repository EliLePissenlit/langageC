/**
 * @file window.c
 * @brief Gestion de la fenêtre et du redimensionnement
 */

#include "./game.h"

void update_window_dimensions(game_t* game, int width, int height) {
    game->dimensions.window_width = width;
    game->dimensions.window_height = height;
    game->dimensions.cell_width = (float)width / N;
    game->dimensions.cell_height = (float)height / N;

    // Met à jour les positions des boutons du menu de victoire
    game->replay_button = (SDL_Rect){
        width / 4,
        2 * height / 3,
        width / 4,
        50
    };

    game->menu_button = (SDL_Rect){
        2 * width / 4,
        2 * height / 3,
        width / 4,
        50
    };
}

void toggle_fullscreen(SDL_Window* window, game_t* game) {
    Uint32 fullscreen_flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    int is_fullscreen = SDL_GetWindowFlags(window) & fullscreen_flag;
    
    SDL_SetWindowFullscreen(window, is_fullscreen ? 0 : fullscreen_flag);
    game->is_fullscreen = !is_fullscreen;
    
    // Met à jour les dimensions
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    update_window_dimensions(game, width, height);
}