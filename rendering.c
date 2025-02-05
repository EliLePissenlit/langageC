// rendering.c
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "./game.h"
#include "./rendering.h"
#include "./logic.h" 

// Définition des couleurs utilisées dans le jeu
const SDL_Color GRID_COLOR = { .r = 255, .g = 255, .b = 255 };          // Couleur de la grille
const SDL_Color PLAYER_X_COLOR = { .r = 255, .g = 50, .b = 50 };        // Rouge pour X
const SDL_Color PLAYER_O_COLOR = { .r = 50, .g = 100, .b = 255 };       // Bleu pour O
const SDL_Color TIE_COLOR = { .r = 100, .g = 100, .b = 100 };          // Gris pour égalité
const SDL_Color VICTORY_BUTTON_COLOR = { .r = 100, .g = 200, .b = 100 }; // Vert pour boutons
const SDL_Color VICTORY_TEXT_COLOR = { .r = 255, .g = 255, .b = 255 };  // Blanc pour texte
const SDL_Color VICTORY_OVERLAY_COLOR = { .r = 0, .g = 0, .b = 0, .a = 180 }; // Fond semi-transparent

// Rendu de la grille de jeu
void render_grid(SDL_Renderer *renderer, const SDL_Color *color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, 255);

    // Dessine les lignes verticales et horizontales
    for (int i = 1; i < N; ++i) {
        SDL_RenderDrawLine(renderer,
                           i * CELL_WIDTH, 0,
                           i * CELL_WIDTH, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer,
                           0,            i * CELL_HEIGHT,
                           SCREEN_WIDTH, i * CELL_HEIGHT);
    }
}

// Rendu d'un X dans une cellule
void render_x(SDL_Renderer *renderer,
              int row, int column,
              const SDL_Color *color)
{
    // Calcule les dimensions et la position du X
    const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
    const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
    const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

    // Dessine les deux lignes du X avec une épaisseur de 10
    thickLineRGBA(renderer,
                  center_x - half_box_side,
                  center_y - half_box_side,
                  center_x + half_box_side,
                  center_y + half_box_side,
                  10,
                  color->r,
                  color->g,
                  color->b,
                  255);
    thickLineRGBA(renderer,
                  center_x + half_box_side,
                  center_y - half_box_side,
                  center_x - half_box_side,
                  center_y + half_box_side,
                  10,
                  color->r,
                  color->g,
                  color->b,
                  255);
}

// Rendu d'un O dans une cellule
void render_o(SDL_Renderer *renderer,
              int row, int column,
              const SDL_Color *color)
{
    // Calcule les dimensions et la position du O
    const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
    const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
    const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

    // Dessine un cercle rempli pour le O
    filledCircleRGBA(renderer,
                     center_x, center_y, half_box_side + 5,
                     color->r, color->g, color->b, 255);
    // Dessine un cercle noir à l'intérieur pour créer l'effet "anneau"
    filledCircleRGBA(renderer,
                     center_x, center_y, half_box_side - 5,
                     0, 0, 0, 255);
}

// Rendu du plateau de jeu complet
void render_board(SDL_Renderer *renderer,
                  const int *board,
                  const SDL_Color *player_x_color,
                  const SDL_Color *player_o_color)
{
    // Parcourt chaque cellule du plateau
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            switch (board[i * N + j]) {
            case PLAYER_X:
                render_x(renderer, i, j, player_x_color);
                break;

            case PLAYER_O:
                render_o(renderer, i, j, player_o_color);
                break;

            default: {}  // Case vide
            }
        }
    }
}

// Rendu du jeu en cours
void render_running_state(SDL_Renderer *renderer, const game_t *game)
{
    render_grid(renderer, &GRID_COLOR);
    render_board(renderer,
                 game->board,
                 &PLAYER_X_COLOR,
                 &PLAYER_O_COLOR);
}

// Rendu de l'état de fin de partie
void render_game_over_state(SDL_Renderer *renderer,
                            const game_t *game,
                            const SDL_Color *color)
{
    render_grid(renderer, color);
    render_board(renderer,
                 game->board,
                 color,
                 color);
}

// Rendu du menu de victoire
void render_victory_menu(SDL_Renderer *renderer, const game_t *game, TTF_Font *font) {
    // Active le mode de transparence
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Crée un overlay semi-transparent
    SDL_SetRenderDrawColor(renderer, 
        VICTORY_OVERLAY_COLOR.r, 
        VICTORY_OVERLAY_COLOR.g, 
        VICTORY_OVERLAY_COLOR.b, 
        VICTORY_OVERLAY_COLOR.a);
    
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    // Détermine le message à afficher
    const char* message;
    if (game->state == VICTORY_MENU_STATE) {
        if (check_player_won(game, PLAYER_X)) {
            message = "Joueur X Gagne!";
        } else if (check_player_won(game, PLAYER_O)) {
            message = "Joueur O Gagne!";
        } else {
            message = "Match Nul!";
        }
    } else {
        message = "Fin de partie!";
    }

    // Rendu du message principal
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, message, VICTORY_TEXT_COLOR);
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = {
            (SCREEN_WIDTH - text_surface->w) / 2,
            SCREEN_HEIGHT / 3 - text_surface->h / 2,
            text_surface->w,
            text_surface->h
        };
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
    }

    // Rendu des boutons
    SDL_SetRenderDrawColor(renderer, 
        VICTORY_BUTTON_COLOR.r, 
        VICTORY_BUTTON_COLOR.g, 
        VICTORY_BUTTON_COLOR.b, 
        255);
    SDL_RenderFillRect(renderer, &game->replay_button);
    SDL_RenderFillRect(renderer, &game->menu_button);

    // Rendu du texte des boutons
    SDL_Surface* replay_text = TTF_RenderText_Solid(font, "Rejouer", VICTORY_TEXT_COLOR);
    SDL_Surface* menu_text = TTF_RenderText_Solid(font, "Menu Principal", VICTORY_TEXT_COLOR);

    if (replay_text && menu_text) {
        // Crée les textures pour les textes des boutons
        SDL_Texture* replay_texture = SDL_CreateTextureFromSurface(renderer, replay_text);
        SDL_Texture* menu_texture = SDL_CreateTextureFromSurface(renderer, menu_text);

        // Position le texte "Rejouer"
        SDL_Rect replay_text_rect = {
            game->replay_button.x + (game->replay_button.w - replay_text->w) / 2,
            game->replay_button.y + (game->replay_button.h - replay_text->h) / 2,
            replay_text->w,
            replay_text->h
        };

        // Position le texte "Menu Principal"
        SDL_Rect menu_text_rect = {
            game->menu_button.x + (game->menu_button.w - menu_text->w) / 2,
            game->menu_button.y + (game->menu_button.h - menu_text->h) / 2,
            menu_text->w,
            menu_text->h
        };

        // Affiche les textes
        SDL_RenderCopy(renderer, replay_texture, NULL, &replay_text_rect);
        SDL_RenderCopy(renderer, menu_texture, NULL, &menu_text_rect);

        // Libère les ressources
        SDL_FreeSurface(replay_text);
        SDL_FreeSurface(menu_text);
        SDL_DestroyTexture(replay_texture);
        SDL_DestroyTexture(menu_texture);
    }
}

// Fonction principale de rendu du jeu
void render_game(SDL_Renderer *renderer, const game_t *game, TTF_Font *font) {
    switch (game->state) {
        case RUNNING_STATE:
            render_running_state(renderer, game);
            break;

        case PLAYER_X_WON_STATE:
        case PLAYER_O_WON_STATE:
        case TIE_STATE:
            render_game_over_state(renderer, game, 
                game->state == PLAYER_X_WON_STATE ? &PLAYER_X_COLOR :
                game->state == PLAYER_O_WON_STATE ? &PLAYER_O_COLOR :
                &TIE_COLOR);
            break;

        default: {}
    }

    // Affiche le menu de victoire si nécessaire
    if (game->state == VICTORY_MENU_STATE) {
        render_victory_menu(renderer, game, font);
    }
}