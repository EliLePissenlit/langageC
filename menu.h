/**
 * @file menu.h
 * @brief Interface pour la gestion des menus du jeu
 *
 * Ce module gère tout l'aspect interface utilisateur du jeu, incluant :
 * - Le menu principal
 * - Le menu de sélection de difficulté
 * - Le menu de sélection du mode de jeu
 * - Les transitions entre les différents états
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
#define BUTTON_WIDTH 200    /**< Largeur des boutons en pixels */
#define BUTTON_HEIGHT 50    /**< Hauteur des boutons en pixels */
#define BUTTON_SPACING 30   /**< Espacement vertical entre les boutons */

/** @brief Configuration de la police */
#define FONT_SIZE 24        /**< Taille de la police en points */

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
    QUIT_STATE = 4         /**< Quitter le jeu */
} MenuState;

/*********************************
 * Structure du menu
 *********************************/

/**
 * @brief Structure principale du menu
 * 
 * Contient tous les éléments nécessaires pour gérer l'interface utilisateur :
 * - État actuel du menu
 * - Zones cliquables des boutons
 * - Police pour le texte
 */
typedef struct {
    MenuState mode;           /**< État actuel du menu */
    SDL_Rect player_button;   /**< Bouton joueur vs joueur */
    SDL_Rect ai_button;       /**< Bouton joueur vs IA */
    SDL_Rect easy_button;     /**< Bouton difficulté facile */
    SDL_Rect medium_button;   /**< Bouton difficulté moyenne */
    SDL_Rect hard_button;     /**< Bouton difficulté difficile */
    SDL_Rect classic_button;  /**< Bouton mode classique */
    SDL_Rect snake_button;    /**< Bouton mode snake */
    TTF_Font* font;          /**< Police pour le texte */
    window_dimensions_t dimensions; /**< Nouvelle dimensions*/
} menu_t;

/*********************************
 * Fonctions publiques
 *********************************/

/**
 * @brief Met à jour les dimensions et positions des boutons du menu
 * 
 * Cette fonction recalcule les positions et tailles des boutons
 * en fonction des nouvelles dimensions de la fenêtre
 *
 * @param menu Pointeur vers la structure du menu
 * @param width Nouvelle largeur de la fenêtre
 * @param height Nouvelle hauteur de la fenêtre
 */
void update_menu_dimensions(menu_t* menu, int width, int height);

/**
 * @brief Initialise la structure du menu
 * 
 * Configure :
 * - L'état initial
 * - La disposition des boutons
 * - La police de caractères
 *
 * @param menu Pointeur vers la structure à initialiser
 */
void init_menu(menu_t* menu);

/**
 * @brief Gère l'affichage du menu
 * 
 * Affiche les éléments appropriés selon l'état actuel :
 * - Menu principal
 * - Sélection de difficulté
 * - Sélection de mode
 *
 * @param renderer Contexte de rendu SDL
 * @param menu État du menu à afficher
 */
void render_menu(SDL_Renderer* renderer, const menu_t* menu);

/**
 * @brief Gère les clics dans le menu
 * 
 * Détermine l'action à effectuer selon la zone cliquée et l'état actuel.
 *
 * @param menu État du menu
 * @param x Coordonnée X du clic
 * @param y Coordonnée Y du clic
 * @return Code correspondant à l'action choisie
 */
int handle_menu_click(menu_t* menu, int x, int y);

/**
 * @brief Réinitialise le menu à son état initial
 * @param menu Menu à réinitialiser
 */
void reset_menu(menu_t* menu);

/**
 * @brief Libère les ressources utilisées par le menu
 * @param menu Menu à nettoyer
 */
void cleanup_menu(menu_t* menu);

/**
 * @brief Affiche le texte d'un bouton
 * 
 * @param renderer Contexte de rendu SDL
 * @param font Police à utiliser
 * @param text Texte à afficher
 * @param button Rectangle définissant le bouton
 */
void render_menu_text(SDL_Renderer* renderer, TTF_Font* font, 
                     const char* text, const SDL_Rect* button);

#endif  /* MENU_H_ */