/**
 * @file menu.c
 * @brief Implémentation de la gestion des menus avec nouveau thème noir/rouge/bleu
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "./menu.h"
#include "./rendering.h"

/*********************************
 * Configuration
 *********************************/

/** @brief Chemin de la police par défaut */
#define DEFAULT_FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

/** @brief Style des boutons */
#define BUTTON_CORNER_RADIUS 8
#define BUTTON_HOVER_SCALE 1.05f

/*********************************
 * Configuration des couleurs
 *********************************/

typedef struct {
    SDL_Color background;
    SDL_Color button;
    SDL_Color button_hover;
    SDL_Color accent1;
    SDL_Color accent2;
    SDL_Color text;
    SDL_Color text_secondary;
} ColorTheme;

static const ColorTheme THEME = {
    .background = {18, 18, 24, 255},      // Noir profond
    .button = {28, 28, 35, 255},          // Gris foncé
    .button_hover = {38, 38, 45, 255},    // Gris plus clair
    .accent1 = {220, 40, 40, 255},        // Rouge
    .accent2 = {40, 100, 220, 255},       // Bleu
    .text = {255, 255, 255, 255},         // Blanc
    .text_secondary = {180, 180, 180, 255} // Gris clair
};

/*********************************
 * Fonctions utilitaires statiques
 *********************************/

/**
 * @brief Dessine un bouton avec des coins arrondis et effets
 */
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

/**
 * @brief Dessine le texte du bouton avec effet d'ombre
 */
static void draw_button_text(SDL_Renderer* renderer, TTF_Font* font,
                           const char* text, const SDL_Rect* button,
                           const SDL_Color* color, int is_hovered) {
    // Ombre du texte
    SDL_Color shadow_color = {0, 0, 0, 180};
    SDL_Surface* shadow_surface = TTF_RenderText_Blended(font, text, shadow_color);
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

/**
 * @brief Dessine le titre d'un menu avec style
 */
static void draw_menu_title(SDL_Renderer* renderer, TTF_Font* font,
                          const char* text, int y_pos, int window_width) {
    // Ligne décorative gauche
    SDL_SetRenderDrawColor(renderer, 
        THEME.accent1.r, THEME.accent1.g, 
        THEME.accent1.b, THEME.accent1.a);
    SDL_RenderDrawLine(renderer, 
        window_width/2 - 200, y_pos + 15,
        window_width/2 - 50, y_pos + 15);

    // Ligne décorative droite
    SDL_SetRenderDrawColor(renderer,
        THEME.accent2.r, THEME.accent2.g, 
        THEME.accent2.b, THEME.accent2.a);
    SDL_RenderDrawLine(renderer,
        window_width/2 + 50, y_pos + 15,
        window_width/2 + 200, y_pos + 15);

    // Titre
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, text, THEME.text);
    if (title_surface) {
        SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
        SDL_Rect title_rect = {
            (window_width - title_surface->w) / 2,
            y_pos,
            title_surface->w,
            title_surface->h
        };
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_FreeSurface(title_surface);
        SDL_DestroyTexture(title_texture);
    }
}


void render_menu(SDL_Renderer* renderer, const menu_t* menu) {
    // Fond avec dégradé subtil
    for (int y = 0; y < menu->dimensions.window_height; y++) {
        float factor = (float)y / menu->dimensions.window_height;
        SDL_SetRenderDrawColor(renderer,
            THEME.background.r + (int)(10 * factor),
            THEME.background.g + (int)(10 * factor),
            THEME.background.b + (int)(10 * factor),
            255);
        SDL_RenderDrawLine(renderer, 0, y, menu->dimensions.window_width, y);
    }

    // Position de la souris pour les effets de survol
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_Point mouse = {mouse_x, mouse_y};

    switch (menu->mode) {
        case MENU_STATE:
            // Titre
            draw_menu_title(renderer, menu->font, "MORPION", 
                          menu->player_button.y - 100,
                          menu->dimensions.window_width);

            // Boutons avec effets de survol
            draw_styled_button(renderer, &menu->player_button, 
                             &THEME.button, &THEME.accent1,
                             SDL_PointInRect(&mouse, &menu->player_button));
            draw_styled_button(renderer, &menu->ai_button,
                             &THEME.button, &THEME.accent2,
                             SDL_PointInRect(&mouse, &menu->ai_button));

            draw_button_text(renderer, menu->font, "Joueur vs Joueur",
                           &menu->player_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->player_button));
            draw_button_text(renderer, menu->font, "Joueur vs IA",
                           &menu->ai_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->ai_button));
            break;

        case AI_DIFFICULTY_STATE:
            // Titre
            draw_menu_title(renderer, menu->font, "DIFFICULTE",
                          menu->easy_button.y - 100,
                          menu->dimensions.window_width);

            // Boutons de difficulté
            draw_styled_button(renderer, &menu->easy_button,
                             &THEME.button, &THEME.accent2,
                             SDL_PointInRect(&mouse, &menu->easy_button));
            draw_styled_button(renderer, &menu->medium_button,
                             &THEME.button, &THEME.accent2,
                             SDL_PointInRect(&mouse, &menu->medium_button));
            draw_styled_button(renderer, &menu->hard_button,
                             &THEME.button, &THEME.accent2,
                             SDL_PointInRect(&mouse, &menu->hard_button));

            draw_button_text(renderer, menu->font, "Facile",
                           &menu->easy_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->easy_button));
            draw_button_text(renderer, menu->font, "Moyen",
                           &menu->medium_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->medium_button));
            draw_button_text(renderer, menu->font, "Difficile",
                           &menu->hard_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->hard_button));
            break;

        case GAME_MODE_STATE:
            // Titre
            draw_menu_title(renderer, menu->font, "MODE DE JEU",
                          menu->classic_button.y - 100,
                          menu->dimensions.window_width);

            // Boutons de mode
            draw_styled_button(renderer, &menu->classic_button,
                             &THEME.button, &THEME.accent1,
                             SDL_PointInRect(&mouse, &menu->classic_button));
            draw_styled_button(renderer, &menu->snake_button,
                             &THEME.button, &THEME.accent1,
                             SDL_PointInRect(&mouse, &menu->snake_button));

            draw_button_text(renderer, menu->font, "Mode Classique",
                           &menu->classic_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->classic_button));
            draw_button_text(renderer, menu->font, "Mode Snake",
                           &menu->snake_button, &THEME.text,
                           SDL_PointInRect(&mouse, &menu->snake_button));
            break;

        default:
            break;
    }
}
void init_menu(menu_t* menu) {
    menu->mode = MENU_STATE;
    
    // Chargement de la police
    menu->font = TTF_OpenFont(DEFAULT_FONT_PATH, FONT_SIZE);
    if (!menu->font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }

    // Initialisation avec les dimensions par défaut
    update_menu_dimensions(menu, INITIAL_WIDTH, INITIAL_HEIGHT);
}

void reset_menu(menu_t* menu) {
    menu->mode = MENU_STATE;
}

void cleanup_menu(menu_t* menu) {
    if (menu->font) {
        TTF_CloseFont(menu->font);
        menu->font = NULL;
    }
}

int handle_menu_click(menu_t* menu, int x, int y) {
    SDL_Point click = { x, y };
    
    switch (menu->mode) {
        case MENU_STATE:
            if (SDL_PointInRect(&click, &menu->player_button)) {
                menu->mode = GAME_MODE_STATE;
                return 1;  // Signal pour mode JcJ
            }
            if (SDL_PointInRect(&click, &menu->ai_button)) {
                menu->mode = AI_DIFFICULTY_STATE;
                return 0;  // Signal pour mode IA
            }
            break;

        case AI_DIFFICULTY_STATE:
            if (SDL_PointInRect(&click, &menu->easy_button)) {
                menu->mode = GAME_STATE;
                return EASY;
            }
            if (SDL_PointInRect(&click, &menu->medium_button)) {
                menu->mode = GAME_STATE;
                return MEDIUM;
            }
            if (SDL_PointInRect(&click, &menu->hard_button)) {
                menu->mode = GAME_STATE;
                return HARD;
            }
            break;

        case GAME_MODE_STATE:
            if (SDL_PointInRect(&click, &menu->classic_button)) {
                menu->mode = GAME_STATE;
                return MODE_CLASSIC;
            }
            if (SDL_PointInRect(&click, &menu->snake_button)) {
                menu->mode = GAME_STATE;
                return MODE_SNAKE;
            }
            break;

        default:
            break;
    }
    
    return -1; // Aucune action spéciale
}

void update_menu_dimensions(menu_t* menu, int width, int height) {
    menu->dimensions.window_width = width;
    menu->dimensions.window_height = height;
    
    int button_width = width / 3;  // Adaptatif à la largeur
    int button_height = height / 10;  // Adaptatif à la hauteur
    int spacing = button_height / 2;
    int x_offset = (width - button_width) / 2;
    int y_base = height / 3;

    // Menu principal
    menu->player_button = (SDL_Rect){x_offset, y_base, button_width, button_height};
    menu->ai_button = (SDL_Rect){x_offset, y_base + button_height + spacing, button_width, button_height};

    // Menu difficulté
    menu->easy_button = (SDL_Rect){x_offset, y_base, button_width, button_height};
    menu->medium_button = (SDL_Rect){x_offset, y_base + button_height + spacing, button_width, button_height};
    menu->hard_button = (SDL_Rect){x_offset, y_base + 2 * (button_height + spacing), button_width, button_height};

    // Menu mode de jeu
    menu->classic_button = (SDL_Rect){x_offset, y_base, button_width, button_height};
    menu->snake_button = (SDL_Rect){x_offset, y_base + button_height + spacing, button_width, button_height};
}