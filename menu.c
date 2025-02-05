// menu.c
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./menu.h"
#include "./rendering.h"

// Couleurs du menu
const SDL_Color MENU_BUTTON_COLOR = { .r = 100, .g = 200, .b = 100 };
const SDL_Color MENU_TEXT_COLOR = { .r = 255, .g = 255, .b = 255 };
const SDL_Color MENU_BACKGROUND_COLOR = { .r = 50, .g = 50, .b = 50 };

void render_menu_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, const SDL_Rect *button) {
    // Crée une surface avec le texte
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, MENU_TEXT_COLOR);
    if (surface == NULL) return;

    // Convertit la surface en texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        SDL_FreeSurface(surface);
        return;
    }

    // Centre le texte dans le bouton
    SDL_Rect text_rect;
    text_rect.w = surface->w;
    text_rect.h = surface->h;
    text_rect.x = button->x + (button->w - text_rect.w) / 2;
    text_rect.y = button->y + (button->h - text_rect.h) / 2;

    // Affiche le texte
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);

    // Nettoie les ressources
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Initialisation du menu et de ses composants
void init_menu(menu_t *menu) {
    menu->mode = MENU_STATE;
    
    // Initialise SDL_TTF
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return;
    }

    // Charge la police
    menu->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", FONT_SIZE);
    if (!menu->font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }
    
    // Calcule la position horizontale des boutons
    int x_offset = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
    int y_base = SCREEN_HEIGHT / 3;
    int y_spacing = BUTTON_HEIGHT + BUTTON_SPACING;
    
    // Boutons du menu principal
    menu->player_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    menu->ai_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base + y_spacing,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    // Boutons de difficulté IA
    menu->easy_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    menu->medium_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base + y_spacing,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    menu->hard_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base + (2 * y_spacing),
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    // Boutons de mode de jeu
    menu->classic_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };

    menu->snake_button = (SDL_Rect) {
        .x = x_offset,
        .y = y_base + y_spacing,
        .w = BUTTON_WIDTH,
        .h = BUTTON_HEIGHT
    };
}

// Rendu du menu
void render_menu(SDL_Renderer *renderer, const menu_t *menu) {
    // Fond du menu
    SDL_SetRenderDrawColor(renderer, 
        MENU_BACKGROUND_COLOR.r,
        MENU_BACKGROUND_COLOR.g, 
        MENU_BACKGROUND_COLOR.b, 
        255);
    SDL_RenderClear(renderer);

    // Couleur des boutons
    SDL_SetRenderDrawColor(renderer, 
        MENU_BUTTON_COLOR.r,
        MENU_BUTTON_COLOR.g, 
        MENU_BUTTON_COLOR.b, 
        255);

    if (menu->mode == MENU_STATE) {
        // Menu principal
        SDL_RenderFillRect(renderer, &menu->player_button);
        SDL_RenderFillRect(renderer, &menu->ai_button);
        
        // Texte des boutons
        render_menu_text(renderer, menu->font, "Joueur vs Joueur", &menu->player_button);
        render_menu_text(renderer, menu->font, "Joueur vs IA", &menu->ai_button);
    } 
    else if (menu->mode == AI_DIFFICULTY_STATE) {
        // Menu de difficulté IA
        SDL_RenderFillRect(renderer, &menu->easy_button);
        SDL_RenderFillRect(renderer, &menu->medium_button);
        SDL_RenderFillRect(renderer, &menu->hard_button);

        // Texte des boutons de difficulté
        render_menu_text(renderer, menu->font, "Facile", &menu->easy_button);
        render_menu_text(renderer, menu->font, "Moyen", &menu->medium_button);
        render_menu_text(renderer, menu->font, "Difficile", &menu->hard_button);
    }
    else if (menu->mode == GAME_MODE_STATE) {
        printf("Rendering game mode selection menu\n");  // Debug log
        
        // Dessine les boutons du menu de mode
        SDL_RenderFillRect(renderer, &menu->classic_button);
        SDL_RenderFillRect(renderer, &menu->snake_button);

        // Vérifie que la police est chargée
        if (!menu->font) {
            printf("Error: Font not loaded!\n");
            return;
        }

        // Dessine les textes des boutons
        render_menu_text(renderer, menu->font, "Mode Classique", &menu->classic_button);
        render_menu_text(renderer, menu->font, "Mode Snake", &menu->snake_button);

        // Titre du menu (optionnel mais aide à la compréhension)
        SDL_Surface* title_surface = TTF_RenderText_Solid(menu->font, 
            "Choisissez votre mode de jeu", 
            MENU_TEXT_COLOR);
        
        if (title_surface) {
            SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
            SDL_Rect title_rect = {
                (SCREEN_WIDTH - title_surface->w) / 2,
                menu->classic_button.y - 60,
                title_surface->w,
                title_surface->h
            };
            SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
            SDL_FreeSurface(title_surface);
            SDL_DestroyTexture(title_texture);
        }
    }
}

// Libération des ressources du menu
void cleanup_menu(menu_t *menu) {
    if (menu->font) {
        TTF_CloseFont(menu->font);
    }
    TTF_Quit();
}

// Gestion des clics dans le menu
int handle_menu_click(menu_t *menu, int x, int y) {
    printf("Menu click handled. Current mode: %d\n", menu->mode);
    
    if (menu->mode == MENU_STATE) {
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->player_button)) {
            menu->mode = GAME_MODE_STATE;
            printf("Selected Player vs Player - Going to game mode selection\n");
            return -1;
        }
        
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->ai_button)) {
            menu->mode = AI_DIFFICULTY_STATE;
            printf("Selected Player vs AI\n");
            return 0;
        }
    } 
    else if (menu->mode == AI_DIFFICULTY_STATE) {
        // Gestion des clics pour la difficulté de l'IA (inchangée)
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->easy_button)) {
            menu->mode = GAME_STATE;
            return EASY;
        }
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->medium_button)) {
            menu->mode = GAME_STATE;
            return MEDIUM;
        }
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->hard_button)) {
            menu->mode = GAME_STATE;
            return HARD;
        }
    }
    else if (menu->mode == GAME_MODE_STATE) {
        // Gestion des clics pour la sélection du mode de jeu
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->classic_button)) {
            menu->mode = GAME_STATE;
            printf("Selected Classic Mode\n");
            return MODE_CLASSIC;
        }
        if (SDL_PointInRect(&(SDL_Point){x, y}, &menu->snake_button)) {
            menu->mode = GAME_STATE;
            printf("Selected Snake Mode\n");
            return MODE_SNAKE;
        }
    }
    
    return -1;
}

// Réinitialisation du menu à son état initial
void reset_menu(menu_t *menu) {
    menu->mode = MENU_STATE;
}