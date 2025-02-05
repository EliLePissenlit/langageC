/**
 * @file rendering.c
 * @brief Implémentation du rendu graphique
 */

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "./game.h"
#include "./rendering.h"
#include "./logic.h"
#include "./window.h"

/*********************************
 * Configuration des styles
 *********************************/

#define BUTTON_CORNER_RADIUS 8
#define BUTTON_HOVER_SCALE 1.05f
#define VICTORY_GLOW_INTENSITY 3
#define VICTORY_TEXT_SCALE 1.5f
#define SYMBOL_THICKNESS 10
#define SYMBOL_SIZE_RATIO 0.25f

typedef struct {
    SDL_Color background;
    SDL_Color button;
    SDL_Color button_hover;
    SDL_Color accent1;
    SDL_Color accent2;
    SDL_Color text;
    SDL_Color text_secondary;
    SDL_Color shadow;
    SDL_Color overlay;
    SDL_Color grid;
} ColorTheme;

static const ColorTheme THEME = {
    .background = {18, 18, 24, 255},      // Noir profond
    .button = {28, 28, 35, 255},          // Gris foncé
    .button_hover = {38, 38, 45, 255},    // Gris plus clair
    .accent1 = {220, 40, 40, 255},        // Rouge (X)
    .accent2 = {40, 100, 220, 255},       // Bleu (O)
    .text = {255, 255, 255, 255},         // Blanc
    .text_secondary = {180, 180, 180, 255}, // Gris clair
    .shadow = {0, 0, 0, 180},             // Ombre semi-transparente
    .overlay = {18, 18, 24, 230},         // Overlay semi-transparent
    .grid = {255, 255, 255, 255}          // Grille blanche
};

/*********************************
 * Fonctions utilitaires statiques
 *********************************/

static void draw_styled_button(SDL_Renderer* renderer, const SDL_Rect* button,
                             const SDL_Color* color, const SDL_Color* border_color,
                             int is_hovered) {
    float scale = is_hovered ? BUTTON_HOVER_SCALE : 1.0f;
    int w = (int)(button->w * scale);
    int h = (int)(button->h * scale);
    int x = button->x - (w - button->w) / 2;
    int y = button->y - (h - button->h) / 2;

    // Fond du bouton avec coins arrondis
    roundedBoxRGBA(renderer,
        x, y, x + w, y + h,
        BUTTON_CORNER_RADIUS,
        color->r, color->g, color->b, color->a);

    // Bordure avec coins arrondis
    roundedRectangleRGBA(renderer,
        x, y, x + w, y + h,
        BUTTON_CORNER_RADIUS,
        border_color->r, border_color->g, border_color->b, border_color->a);

    // Effet de lueur si survolé
    if (is_hovered) {
        for (int i = 1; i <= 3; i++) {
            roundedRectangleRGBA(renderer,
                x - i, y - i,
                x + w + i, y + h + i,
                BUTTON_CORNER_RADIUS + i,
                border_color->r, border_color->g, border_color->b, 100 - i * 30);
        }
    }
}

static void draw_victory_text(SDL_Renderer* renderer, TTF_Font* font,
                            const char* text, int y_pos, int window_width,
                            const SDL_Color* color) {
    // Ombre du texte
    SDL_Surface* shadow_surface = TTF_RenderText_Blended(font, text, THEME.shadow);
    if (shadow_surface) {
        SDL_Texture* shadow_texture = SDL_CreateTextureFromSurface(renderer, shadow_surface);
        SDL_Rect shadow_rect = {
            (window_width - shadow_surface->w) / 2 + 2,
            y_pos + 2,
            shadow_surface->w,
            shadow_surface->h
        };
        SDL_RenderCopy(renderer, shadow_texture, NULL, &shadow_rect);
        SDL_DestroyTexture(shadow_texture);
        SDL_FreeSurface(shadow_surface);
    }

    // Texte principal avec mise à l'échelle
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, *color);
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = {
            (window_width - (int)(text_surface->w * VICTORY_TEXT_SCALE)) / 2,
            y_pos,
            (int)(text_surface->w * VICTORY_TEXT_SCALE),
            (int)(text_surface->h * VICTORY_TEXT_SCALE)
        };
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

static void draw_button_text(SDL_Renderer* renderer, TTF_Font* font,
                           const char* text, const SDL_Rect* button,
                           const SDL_Color* color, int is_hovered) {
    // Ombre du texte
    SDL_Surface* shadow_surface = TTF_RenderText_Blended(font, text, THEME.shadow);
    if (shadow_surface) {
        SDL_Texture* shadow_texture = SDL_CreateTextureFromSurface(renderer, shadow_surface);
        SDL_Rect shadow_rect = {
            button->x + (button->w - shadow_surface->w) / 2 + 2,
            button->y + (button->h - shadow_surface->h) / 2 + 2,
            shadow_surface->w,
            shadow_surface->h
        };
        SDL_RenderCopy(renderer, shadow_texture, NULL, &shadow_rect);
        SDL_DestroyTexture(shadow_texture);
        SDL_FreeSurface(shadow_surface);
    }

    // Texte principal
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, *color);
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = {
            button->x + (button->w - text_surface->w) / 2,
            button->y + (button->h - text_surface->h) / 2,
            text_surface->w,
            text_surface->h
        };
        
        // Effet de mise à l'échelle au survol
        if (is_hovered) {
            text_rect.x -= (int)(text_surface->w * (BUTTON_HOVER_SCALE - 1) / 2);
            text_rect.y -= (int)(text_surface->h * (BUTTON_HOVER_SCALE - 1) / 2);
            text_rect.w = (int)(text_surface->w * BUTTON_HOVER_SCALE);
            text_rect.h = (int)(text_surface->h * BUTTON_HOVER_SCALE);
        }
        
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

/*********************************
 * Fonctions de rendu du jeu
 *********************************/

static void render_grid(SDL_Renderer* renderer, const game_t* game, const SDL_Color* color) {
    // Utilise la couleur passée en paramètre ou la couleur par défaut de la grille
    const SDL_Color* grid_color = color ? color : &THEME.grid;
    
    SDL_SetRenderDrawColor(renderer, 
        grid_color->r, 
        grid_color->g, 
        grid_color->b, 
        grid_color->a);

    // Lignes verticales
    for (int i = 1; i < N; ++i) {
        SDL_RenderDrawLine(renderer,
            i * game->dimensions.cell_width, 0,
            i * game->dimensions.cell_width, game->dimensions.window_height);
    }

    // Lignes horizontales
    for (int i = 1; i < N; ++i) {
        SDL_RenderDrawLine(renderer,
            0, i * game->dimensions.cell_height,
            game->dimensions.window_width, i * game->dimensions.cell_height);
    }
}

static void render_x(SDL_Renderer* renderer, const game_t* game, int row, int column, const SDL_Color* color) {
    const float half_box_side = fmin(game->dimensions.cell_width, game->dimensions.cell_height) * SYMBOL_SIZE_RATIO;
    const float center_x = game->dimensions.cell_width * 0.5 + column * game->dimensions.cell_width;
    const float center_y = game->dimensions.cell_height * 0.5 + row * game->dimensions.cell_height;

    thickLineRGBA(renderer,
        center_x - half_box_side,
        center_y - half_box_side,
        center_x + half_box_side,
        center_y + half_box_side,
        SYMBOL_THICKNESS,
        color->r, color->g, color->b, color->a);

    thickLineRGBA(renderer,
        center_x + half_box_side,
        center_y - half_box_side,
        center_x - half_box_side,
        center_y + half_box_side,
        SYMBOL_THICKNESS,
        color->r, color->g, color->b, color->a);
}

static void render_o(SDL_Renderer* renderer, const game_t* game, int row, int column, const SDL_Color* color) {
    const float half_box_side = fmin(game->dimensions.cell_width, game->dimensions.cell_height) * SYMBOL_SIZE_RATIO;
    const float center_x = game->dimensions.cell_width * 0.5 + column * game->dimensions.cell_width;
    const float center_y = game->dimensions.cell_height * 0.5 + row * game->dimensions.cell_height;

    filledCircleRGBA(renderer,
        center_x, center_y,
        half_box_side + 5,
        color->r, color->g, color->b, color->a);

    filledCircleRGBA(renderer,
        center_x, center_y,
        half_box_side - 5,
        THEME.background.r, THEME.background.g, THEME.background.b, THEME.background.a);
}

static void render_board(SDL_Renderer* renderer, const game_t* game, const SDL_Color* override_color) {
    const SDL_Color* x_color = override_color ? override_color : &THEME.accent1;
    const SDL_Color* o_color = override_color ? override_color : &THEME.accent2;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            switch (game->board[i * N + j]) {
                case PLAYER_X:
                    render_x(renderer, game, i, j, x_color);
                    break;
                case PLAYER_O:
                    render_o(renderer, game, i, j, o_color);
                    break;
                default:
                    break;
            }
        }
    }
}

static void render_running_state(SDL_Renderer* renderer, const game_t* game) {
    SDL_SetRenderDrawColor(renderer, 
        THEME.background.r, THEME.background.g, 
        THEME.background.b, THEME.background.a);
    SDL_RenderClear(renderer);

    render_grid(renderer, game, NULL);
    render_board(renderer, game, NULL);
}

static void render_game_over_state(SDL_Renderer* renderer, const game_t* game, const SDL_Color* color) {
    SDL_SetRenderDrawColor(renderer, 
        THEME.background.r, THEME.background.g, 
        THEME.background.b, THEME.background.a);
    SDL_RenderClear(renderer);

    render_grid(renderer, game, color);
    render_board(renderer, game, color);
}

void render_victory_menu(SDL_Renderer* renderer, const game_t* game, TTF_Font* font) {
    // Activation du mode de fusion pour les effets de transparence
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Fond avec dégradé subtil
    for (int y = 0; y < game->dimensions.window_height; y++) {
        float factor = (float)y / game->dimensions.window_height;
        SDL_SetRenderDrawColor(renderer,
            THEME.overlay.r + (int)(10 * factor),
            THEME.overlay.g + (int)(10 * factor),
            THEME.overlay.b + (int)(10 * factor),
            THEME.overlay.a);
        SDL_RenderDrawLine(renderer, 0, y, game->dimensions.window_width, y);
    }

    // Détermination des couleurs selon le résultat
    const SDL_Color* accent_color;
    const char* message;
    if (check_player_won(game, PLAYER_X)) {
        accent_color = &THEME.accent1;
        message = "VICTOIRE JOUEUR X";
    } else if (check_player_won(game, PLAYER_O)) {
        accent_color = &THEME.accent2;
        message = "VICTOIRE JOUEUR O";
    } else {
        accent_color = &THEME.text_secondary;
        message = "MATCH NUL";
    }

    // Position de la souris pour les effets de survol
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_Point mouse = {mouse_x, mouse_y};

    // Texte de victoire avec effet de lueur
    int title_y = game->dimensions.window_height / 4;
    
    // Effet de lueur derrière le texte
    for (int i = VICTORY_GLOW_INTENSITY; i > 0; i--) {
        roundedBoxRGBA(renderer,
            game->dimensions.window_width/4 - i*4,
            title_y - 30 - i*4,
            3*game->dimensions.window_width/4 + i*4,
            title_y + 30 + i*4,
            BUTTON_CORNER_RADIUS + i,
            accent_color->r, accent_color->g, accent_color->b, 20/i);
    }

    // Texte de victoire
    draw_victory_text(renderer, font, message, title_y,
                     game->dimensions.window_width, accent_color);

    // Boutons
    SDL_bool replay_hovered = SDL_PointInRect(&mouse, &game->replay_button);
    SDL_bool menu_hovered = SDL_PointInRect(&mouse, &game->menu_button);

    // Bouton Rejouer
    draw_styled_button(renderer, &game->replay_button,
                      &THEME.button, accent_color, replay_hovered);
    draw_button_text(renderer, font, "Rejouer",
                    &game->replay_button, &THEME.text, replay_hovered);

    // Bouton Menu
    draw_styled_button(renderer, &game->menu_button,
                      &THEME.button, accent_color, menu_hovered);
    draw_button_text(renderer, font, "Menu Principal",
                    &game->menu_button, &THEME.text, menu_hovered);
}

void render_game(SDL_Renderer* renderer, const game_t* game, TTF_Font* font) {
    switch (game->state) {
        case RUNNING_STATE:
            render_running_state(renderer, game);
            break;
        case PLAYER_X_WON_STATE:
            render_game_over_state(renderer, game, &THEME.accent1);
            break;
        case PLAYER_O_WON_STATE:
            render_game_over_state(renderer, game, &THEME.accent2);
            break;
        case TIE_STATE:
            render_game_over_state(renderer, game, &THEME.text_secondary);
            break;
        case VICTORY_MENU_STATE:
            // On détermine la couleur à utiliser selon le gagnant
            const SDL_Color* victory_color = NULL;
            if (check_player_won(game, PLAYER_X)) {
                victory_color = &THEME.accent1;
            } else if (check_player_won(game, PLAYER_O)) {
                victory_color = &THEME.accent2;
            } else {
                victory_color = &THEME.text_secondary;
            }
            render_game_over_state(renderer, game, victory_color);
            render_victory_menu(renderer, game, font);
            break;
        default:
            break;
    }
}