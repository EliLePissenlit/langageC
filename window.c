/**
 * @file window.c
 * @brief Gestion de la fenêtre et du redimensionnement
 */

#include "./game.h"

/**
 * @brief Met à jour les dimensions de la fenêtre et recalcule les éléments dépendants
 * 
 * Ajuste :
 * - Les dimensions de la fenêtre
 * - La taille des cellules
 * - La position des boutons du menu de victoire
 *
 * @param game État du jeu à mettre à jour
 * @param width Nouvelle largeur de la fenêtre
 * @param height Nouvelle hauteur de la fenêtre 
 */
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

/**
 * @brief Bascule entre le mode plein écran et le mode fenêtré
 * 
 * Met à jour les dimensions et recalcule les positions des éléments
 * après le changement de mode d'affichage.
 *
 * @param window Fenêtre SDL à modifier
 * @param game État du jeu à mettre à jour
 */
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