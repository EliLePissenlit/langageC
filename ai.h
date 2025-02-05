/**
 * @file ai.h
 * @brief Interface pour l'intelligence artificielle du Tic-Tac-Toe
 *
 * Ce module implémente différents niveaux d'IA :
 * - Facile : Stratégie aléatoire
 * - Moyen : Mélange de stratégie basique et aléatoire
 * - Difficile : Algorithme Minimax
 */

#ifndef AI_H_
#define AI_H_

#include "./game.h"

/**
 * @brief Niveaux de difficulté de l'IA
 * 
 * Les valeurs sont intentionnellement non consécutives pour éviter
 * toute confusion avec d'autres constantes du jeu.
 */
typedef enum {
    EASY = 11,    /**< Coups aléatoires uniquement */
    MEDIUM = 22,  /**< Stratégie simple + aléatoire */
    HARD = 33     /**< Algorithme Minimax (optimal) */
} AIDifficulty;

/**
 * @brief Scores utilisés par l'algorithme Minimax
 */
typedef enum {
    SCORE_LOSS = -10,  /**< Score d'une défaite */
    SCORE_DRAW = 0,    /**< Score d'une égalité */
    SCORE_WIN = 10     /**< Score d'une victoire */
} AIScore;

/**
 * @brief Configuration de l'IA
 */
typedef struct {
    AIDifficulty difficulty;  /**< Niveau de difficulté choisi */
} ai_t;

/*********************************
 * Fonctions principales
 *********************************/

/**
 * @brief Initialise l'IA avec un niveau de difficulté
 * 
 * Configure l'IA et initialise le générateur de nombres aléatoires
 * pour les stratégies qui en ont besoin.
 *
 * @param ai Pointeur vers la structure de l'IA
 * @param difficulty Niveau de difficulté souhaité
 */
void init_ai(ai_t* ai, AIDifficulty difficulty);

/**
 * @brief Fait jouer l'IA
 * 
 * Détermine et joue le meilleur coup selon le niveau de difficulté :
 * - EASY : Coup aléatoire
 * - MEDIUM : Stratégie simple ou aléatoire
 * - HARD : Meilleur coup possible (Minimax)
 *
 * @param game Pointeur vers l'état du jeu
 * @param ai Pointeur vers la configuration de l'IA
 */
void ai_make_move(game_t* game, ai_t* ai);

#endif  /* AI_H_ */