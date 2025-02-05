// ai.h
#ifndef AI_H_
#define AI_H_

#include "./game.h"

// Définition des niveaux de difficulté de l'IA
typedef enum {
    EASY = 11,    // Mode facile - coups aléatoires
    MEDIUM = 22,  // Mode moyen - combine stratégie simple et aléatoire
    HARD = 33     // Mode difficile - utilise l'algorithme minimax
} AIDifficulty;

// Structure représentant l'IA avec son niveau de difficulté
typedef struct {
    AIDifficulty difficulty;
} ai_t;

// Initialise l'IA avec un niveau de difficulté spécifié
void init_ai(ai_t *ai, AIDifficulty difficulty);

// Fait jouer l'IA en fonction de son niveau de difficulté
void ai_make_move(game_t *game, ai_t *ai);

#endif  // AI_H_