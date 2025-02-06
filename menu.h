/**
 * @file menu.h
 * @brief Interface pour la gestion des menus du jeu
 */

#ifndef MENU_H_
#define MENU_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./game.h"
#include "./ai.h"
#include "./window.h"

/*********************************
 * Configuration des menus
 *********************************/

/** @brief Dimensions standards des boutons */
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define BUTTON_SPACING 30

/** @brief Configuration de la police */
#define FONT_SIZE 24

/*********************************
 * États des menus
 *********************************/

/** @brief Modes de jeu spéciaux */
typedef enum {
    MODE_CLASSIC = 20,  /**< Mode de jeu classique */
    MODE_SNAKE = 21     /**< Mode avec mini-jeu Snake */
} GameMode;

/** @brief États possibles du menu */
typedef enum {
    MENU_STATE = 0,          /**< Menu principal */
    GAME_STATE = 1,          /**< Jeu en cours */
    AI_DIFFICULTY_STATE = 2,  /**< Sélection difficulté IA */
    GAME_MODE_STATE = 3,      /**< Sélection mode de jeu */
    SCOREBOARD_STATE = 4,     /**< Tableau des scores */
    QUIT_STATE = 5           /**< Quitter le jeu */
} MenuState;

/*********************************
 * Structure du menu
 *********************************/
#ifdef __cplusplus
extern "C" {
#endif

struct menu_s {
    MenuState mode;           /**< État actuel du menu */
    SDL_Rect player_button;   /**< Bouton joueur vs joueur */
    SDL_Rect ai_button;       /**< Bouton joueur vs IA */
    SDL_Rect scores_button;   /**< Bouton tableau des scores */
    SDL_Rect easy_button;     /**< Bouton difficulté facile */
    SDL_Rect medium_button;   /**< Bouton difficulté moyenne */
    SDL_Rect hard_button;     /**< Bouton difficulté difficile */
    SDL_Rect classic_button;  /**< Bouton mode classique */
    SDL_Rect snake_button;    /**< Bouton mode snake */
    SDL_Rect back_button;     /**< Bouton retour menu */
    TTF_Font* font;          /**< Police pour le texte */
    window_dimensions_t dimensions; /**< Dimensions */
    const game_t* game;      /**< Pointeur vers l'état du jeu */
};

typedef struct menu_s menu_t;

#ifdef __cplusplus
}
#endif

/*********************************
 * Fonctions publiques
 *********************************/

void update_menu_dimensions(menu_t* menu, int width, int height);
void init_menu(menu_t* menu);
void render_menu(SDL_Renderer* renderer, const menu_t* menu);
int handle_menu_click(menu_t* menu, int x, int y);
void reset_menu(menu_t* menu);
void cleanup_menu(menu_t* menu);
void render_menu_text(SDL_Renderer* renderer, TTF_Font* font, 
                     const char* text, const SDL_Rect* button);

#endif  /* MENU_H_ */