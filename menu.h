// menu.h
#ifndef MENU_H_
#define MENU_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./game.h"
#include "./ai.h"

// Constantes pour la configuration du menu
#define BUTTON_WIDTH 200    // Largeur des boutons
#define BUTTON_HEIGHT 50    // Hauteur des boutons
#define BUTTON_SPACING 30   // Espacement entre les boutons
#define FONT_SIZE 24       // Taille de la police

// Modes de jeu spéciaux (valeurs différentes des difficultés de l'IA)
#define MODE_CLASSIC 20
#define MODE_SNAKE 21

// États possibles du menu
typedef enum {
    MENU_STATE = 0,          // Menu principal
    GAME_STATE = 1,          // Jeu en cours
    AI_DIFFICULTY_STATE = 2,  // Sélection de la difficulté de l'IA
    GAME_MODE_STATE = 3      // Sélection du mode (classique/snake)
} GameMode;

// Structure du menu
typedef struct {
    GameMode mode;           // État actuel du menu
    SDL_Rect player_button;  // Bouton joueur vs joueur
    SDL_Rect ai_button;      // Bouton joueur vs IA
    SDL_Rect easy_button;    // Bouton difficulté facile
    SDL_Rect medium_button;  // Bouton difficulté moyenne
    SDL_Rect hard_button;    // Bouton difficulté difficile
    SDL_Rect classic_button; // Bouton mode classique
    SDL_Rect snake_button;   // Bouton mode snake
    TTF_Font* font;         // Police pour le texte
} menu_t;

// Fonctions de gestion du menu
void init_menu(menu_t *menu);
void render_menu(SDL_Renderer *renderer, const menu_t *menu);
int handle_menu_click(menu_t *menu, int x, int y);
void reset_menu(menu_t *menu);
void cleanup_menu(menu_t *menu);
void render_menu_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, const SDL_Rect *button);

#endif  // MENU_H_