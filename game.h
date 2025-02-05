/**
 * @file game.h
 * @brief Définitions fondamentales pour le jeu Tic-Tac-Toe
 *
 * Ce fichier contient les constantes, énumérations et structures de base
 * utilisées dans l'ensemble du projet. Il définit la configuration du jeu,
 * les états possibles, et la structure principale maintenant l'état du jeu.
 */

#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>

/*********************************
 * Configuration de la grille
 *********************************/
/** @brief Taille de la grille (3x3 pour un morpion standard) */
#define N 3

/*********************************
 * Configuration de l'affichage
 *********************************/
/** @brief Largeur de la fenêtre de jeu en pixels */
#define SCREEN_WIDTH 640.0
/** @brief Hauteur de la fenêtre de jeu en pixels */
#define SCREEN_HEIGHT 480.0
#define MIN_WINDOW_SIZE 300


// /** @brief Largeur d'une cellule de la grille */
// #define CELL_WIDTH (SCREEN_WIDTH / N)
// /** @brief Hauteur d'une cellule de la grille */
// #define CELL_HEIGHT (SCREEN_HEIGHT / N)

typedef struct {
    int window_width;
    int window_height;
    float cell_width;
    float cell_height;
} window_dimensions_t;


/*********************************
 * Configuration du timing
 *********************************/
/** 
 * @brief Durée d'affichage de l'écran de victoire en millisecondes
 * Définit combien de temps l'état final est affiché avant le menu de victoire
 */
#define VICTORY_DISPLAY_TIME 1500

/*********************************
 * États des cellules
 *********************************/
/**
 * @brief États possibles pour une cellule du plateau
 * @note Utilise des valeurs explicites pour la clarté et la débuggabilité
 */
typedef enum {
    EMPTY = 0,    /**< Case vide */
    PLAYER_X = 1, /**< Case marquée d'une croix */
    PLAYER_O = 2  /**< Case marquée d'un rond */
} CellState;

/*********************************
 * États du jeu
 *********************************/
/**
 * @brief États possibles pour le jeu
 * Définit les différentes phases possibles d'une partie
 */
typedef enum {
    RUNNING_STATE = 0,      /**< Partie en cours */
    PLAYER_X_WON_STATE = 1, /**< Victoire du joueur X */
    PLAYER_O_WON_STATE = 2, /**< Victoire du joueur O */
    TIE_STATE = 3,          /**< Match nul */
    VICTORY_MENU_STATE = 4, /**< Menu de fin de partie */
} GameState;

/*********************************
 * Structure principale du jeu
 *********************************/
/**
 * @brief Structure principale maintenant l'état du jeu
 * 
 * Cette structure contient toutes les informations nécessaires
 * pour représenter l'état complet d'une partie à un instant donné
 */
typedef struct {
    /** @brief Plateau de jeu (tableau 1D représentant une grille 2D) */
    int board[N * N];
    
    /** @brief Joueur actif (PLAYER_X ou PLAYER_O) */
    int player;
    
    /** @brief État actuel du jeu */
    GameState state;
    
    /** 
     * @brief Horodatage de la victoire
     * Utilisé pour la temporisation de l'affichage de fin de partie
     */
    Uint32 victory_time;
    
    /** @brief Zone cliquable pour le bouton "Rejouer" */
    SDL_Rect replay_button;
    
    /** @brief Zone cliquable pour le bouton "Menu principal" */
    SDL_Rect menu_button;
    
    /** 
     * @brief Mode de jeu actif
     * true = mode Snake, false = mode classique
     */
    int is_snake_mode;
    
    /**
     * @brief Mode IA actif
     * true = partie contre l'IA, false = partie JcJ
     */
    int is_ai_mode;

    int is_fullscreen;
    window_dimensions_t dimensions;
} game_t;

/*********************************
 * Macros utilitaires
 *********************************/
/**
 * @brief Convertit des coordonnées 2D en index 1D
 * @param row Numéro de ligne
 * @param col Numéro de colonne
 * @return Index dans le tableau board[]
 */
#define BOARD_INDEX(row, col) ((row) * N + (col))

/**
 * @brief Vérifie si des coordonnées sont valides
 * @param row Numéro de ligne
 * @param col Numéro de colonne
 * @return true si les coordonnées sont dans la grille
 */
#define IS_VALID_CELL(row, col) ((row) >= 0 && (row) < N && (col) >= 0 && (col) < N)

#endif  /* GAME_H_ */