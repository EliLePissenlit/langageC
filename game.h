/**
 * @file game.h
 * @brief Définitions fondamentales pour le jeu Tic-Tac-Toe
 */

#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include "./score.h"

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
 */
#define VICTORY_DISPLAY_TIME 1500

/*********************************
 * États des cellules
 *********************************/
typedef enum {
    EMPTY = 0,    /**< Case vide */
    PLAYER_X = 1, /**< Case marquée d'une croix */
    PLAYER_O = 2  /**< Case marquée d'un rond */
} CellState;

/*********************************
 * États du jeu
 *********************************/
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
typedef struct game_t {
    /** @brief Plateau de jeu (tableau 1D représentant une grille 2D) */
    int board[N * N];
    
    /** @brief Joueur actif (PLAYER_X ou PLAYER_O) */
    int player;
    
    /** @brief État actuel du jeu */
    GameState state;
    
    /** @brief Score de la session courante */
    session_score_t current_score;
    
    /** @brief Historique des scores */
    score_history_t* score_history;

    /** @brief État du système de score */
    ScoreState score_state;
    
    /** @brief Buffer pour la saisie du pseudo */
    char username_buffer[MAX_USERNAME_INPUT_LENGTH];
    
    /** @brief Horodatage de la victoire */
    Uint32 victory_time;
    
    /** @brief Zone cliquable pour le bouton "Rejouer" */
    SDL_Rect replay_button;
    
    /** @brief Zone cliquable pour le bouton "Menu principal" */
    SDL_Rect menu_button;
    
    /** @brief Mode de jeu actif */
    int is_snake_mode;
    
    /** @brief Mode IA actif */
    int is_ai_mode;

    int is_fullscreen;
    window_dimensions_t dimensions;
} game_t;

/*********************************
 * Macros utilitaires
 *********************************/
#define BOARD_INDEX(row, col) ((row) * N + (col))
#define IS_VALID_CELL(row, col) ((row) >= 0 && (row) < N && (col) >= 0 && (col) < N)

#endif  /* GAME_H_ */