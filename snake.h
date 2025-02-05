/**
 * @file snake.h
 * @brief Interface pour le mini-jeu Snake intégré au Tic-Tac-Toe
 *
 * Ce module implémente un mini-jeu Snake qui sert de condition
 * pour placer un symbole dans le Tic-Tac-Toe. Le joueur doit
 * atteindre un score minimum dans le Snake pour pouvoir jouer son coup.
 */

#ifndef SNAKE_H_
#define SNAKE_H_

#include <SDL2/SDL.h>
#include "./game.h"
#include "./window.h"

/*********************************
 * Configuration du Snake
 *********************************/

/** @brief Dimensions de la grille du Snake */
#define SNAKE_GRID_SIZE 20

/** @brief Score nécessaire pour gagner */
#define SNAKE_MIN_SCORE 5

/** @brief Délai entre deux mouvements (ms) */
#define SNAKE_SPEED 100

/*********************************
 * Énumérations
 *********************************/

/**
 * @brief États possibles du mini-jeu Snake
 */
typedef enum {
    SNAKE_NONE,     /**< État initial/invalide */
    SNAKE_RUNNING,  /**< Partie en cours */
    SNAKE_WON,      /**< Victoire (score atteint) */
    SNAKE_LOST      /**< Défaite (collision) */
} SnakeState;

/**
 * @brief Directions possibles du serpent
 */
typedef enum {
    SNAKE_UP,     /**< Déplacement vers le haut */
    SNAKE_DOWN,   /**< Déplacement vers le bas */
    SNAKE_LEFT,   /**< Déplacement vers la gauche */
    SNAKE_RIGHT   /**< Déplacement vers la droite */
} SnakeDirection;

/*********************************
 * Structures
 *********************************/

/**
 * @brief Segment du corps du serpent
 */
typedef struct SnakeSegment {
    int x;                     /**< Position X sur la grille */
    int y;                     /**< Position Y sur la grille */
    struct SnakeSegment* next; /**< Segment suivant */
} SnakeSegment;

/**
 * @brief État complet du jeu Snake
 */
typedef struct {
    SnakeSegment* head;      /**< Tête du serpent */
    SnakeSegment* tail;      /**< Queue du serpent */
    SnakeDirection direction; /**< Direction actuelle */
    int apple_x;             /**< Position X de la pomme */
    int apple_y;             /**< Position Y de la pomme */
    int score;              /**< Score actuel */
    SnakeState state;       /**< État du jeu */
    float zoom_factor;      /**< Facteur de zoom pour transition */
    SDL_Point target_cell;  /**< Case du morpion ciblée */
    window_dimensions_t dimensions; /**< Nouvelles dimensions*/
} snake_t;

/*********************************
 * Fonctions publiques
 *********************************/


/**
 * @brief Initialise une nouvelle partie de Snake
 *
 * @param snake Structure à initialiser
 * @param target_row Ligne ciblée dans le morpion
 * @param target_col Colonne ciblée dans le morpion
 * @param dimensions Dimensions actuelles de la fenêtre
 */
void init_snake_game(snake_t* snake, int target_row, int target_col, 
                    const window_dimensions_t* dimensions);

/**
 * @brief Met à jour l'état du jeu
 *
 * Gère :
 * - Le déplacement du serpent
 * - La collecte des pommes
 * - Les collisions
 * - Le score
 *
 * @param snake État du jeu à mettre à jour
 */
void update_snake(snake_t* snake);

/**
 * @brief Gère l'affichage du jeu
 *
 * @param renderer Contexte de rendu SDL
 * @param snake État du jeu à afficher
 */
void render_snake_game(SDL_Renderer* renderer, const snake_t* snake);

/**
 * @brief Gère les entrées utilisateur
 *
 * @param event Événement SDL à traiter
 * @param snake État du jeu
 * @return 1 pour continuer, 0 pour quitter
 */
int handle_snake_input(SDL_Event* event, snake_t* snake);

/**
 * @brief Nettoie les ressources du jeu
 *
 * @param snake Structure à nettoyer
 */
void cleanup_snake(snake_t* snake);

/**
 * @brief Gère l'animation de transition
 *
 * @param snake État du jeu
 * @param dt Delta temps depuis la dernière mise à jour
 */
void animate_snake_transition(snake_t* snake, float dt);

/**
 * @brief Lance une partie de Snake pour valider un coup au morpion
 *
 * Fonction principale qui :
 * 1. Initialise le mini-jeu
 * 2. Gère la boucle de jeu
 * 3. Retourne le résultat
 *
 * @param game État du morpion
 * @param renderer Contexte de rendu SDL
 * @param row Ligne ciblée
 * @param col Colonne ciblée
 * @return État final du Snake (gagné/perdu)
 */
SnakeState play_snake_minigame(game_t* game, SDL_Renderer* renderer, 
                              int row, int col);

#endif  /* SNAKE_H_ */