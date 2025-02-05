/**
 * @file ai.c
 * @brief Implémentation de l'intelligence artificielle
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./ai.h"
#include "./logic.h"

/*********************************
 * Fonctions utilitaires statiques
 *********************************/

/**
 * @brief Renvoie la valeur maximale entre deux entiers
 */
static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * @brief Renvoie la valeur minimale entre deux entiers
 */
static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

/**
 * @brief Vérifie si un coup permet une victoire immédiate
 * 
 * @param game État du jeu
 * @param position Position à tester
 * @param player Joueur à vérifier
 * @return 1 si le coup est gagnant, 0 sinon
 */
static int is_winning_move(game_t* game, int position, int player) {
    if (game->board[position] != EMPTY) {
        return 0;
    }
    
    // Simule le coup
    int original = game->board[position];
    game->board[position] = player;
    int result = check_player_won(game, player);
    
    // Restaure l'état
    game->board[position] = original;
    
    return result;
}

/**
 * @brief Trouve une case vide aléatoire
 * 
 * @param game État du jeu
 * @return Position choisie, -1 si aucune case disponible
 */
static int find_random_empty_cell(const game_t* game) {
    int empty_cells[N * N];
    int count = 0;

    // Collecte des cases vides
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            empty_cells[count++] = i;
        }
    }

    return (count > 0) ? empty_cells[rand() % count] : -1;
}

/**
 * @brief Évalue la position actuelle pour Minimax
 * 
 * @param game État du jeu
 * @return Score de la position (-10 à +10)
 */
static int evaluate_board(const game_t* game) {
    if (check_player_won(game, PLAYER_O)) return SCORE_WIN;    // Victoire IA
    if (check_player_won(game, PLAYER_X)) return SCORE_LOSS;   // Victoire joueur
    return SCORE_DRAW;  // Match nul ou partie en cours
}

/**
 * @brief Implémentation de l'algorithme Minimax
 * 
 * @param game État du jeu
 * @param depth Profondeur actuelle
 * @param is_maximizing true pour maximiser, false pour minimiser
 * @return Meilleur score possible depuis cette position
 */
static int minimax(game_t* game, int depth, int is_maximizing) {
    int score = evaluate_board(game);

    // Cas de base : position finale
    if (score != SCORE_DRAW || count_cells(game->board, EMPTY) == 0) {
        return score;
    }

    if (is_maximizing) {
        // Tour de l'IA (maximise le score)
        int best = SCORE_LOSS - 1;
        for (int i = 0; i < N * N; i++) {
            if (game->board[i] == EMPTY) {
                game->board[i] = PLAYER_O;
                best = max(best, minimax(game, depth + 1, 0));
                game->board[i] = EMPTY;
            }
        }
        return best;
    } else {
        // Tour du joueur (minimise le score)
        int best = SCORE_WIN + 1;
        for (int i = 0; i < N * N; i++) {
            if (game->board[i] == EMPTY) {
                game->board[i] = PLAYER_X;
                best = min(best, minimax(game, depth + 1, 1));
                game->board[i] = EMPTY;
            }
        }
        return best;
    }
}

/**
 * @brief Trouve le meilleur coup possible (mode difficile)
 * 
 * Utilise l'algorithme Minimax pour évaluer toutes les possibilités
 * et choisir le meilleur coup.
 *
 * @param game État du jeu
 * @return Position du meilleur coup
 */
static int find_best_move(game_t* game) {
    int best_value = SCORE_LOSS - 1;
    int best_move = -1;

    // Évalue chaque coup possible
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            game->board[i] = PLAYER_O;
            int move_value = minimax(game, 0, 0);
            game->board[i] = EMPTY;

            if (move_value > best_value) {
                best_move = i;
                best_value = move_value;
            }
        }
    }

    return best_move;
}

/**
 * @brief Trouve un coup pour le niveau moyen
 * 
 * Stratégie :
 * 1. Cherche une victoire immédiate
 * 2. Bloque une victoire adverse imminente
 * 3. Joue aléatoirement
 *
 * @param game État du jeu
 * @return Position choisie
 */
static int find_medium_move(game_t* game) {
    // 1. Recherche d'une victoire immédiate
    for (int i = 0; i < N * N; i++) {
        if (is_winning_move(game, i, PLAYER_O)) {
            return i;
        }
    }

    // 2. Blocage d'une victoire adverse
    for (int i = 0; i < N * N; i++) {
        if (is_winning_move(game, i, PLAYER_X)) {
            return i;
        }
    }

    // 3. Coup aléatoire
    return find_random_empty_cell(game);
}

/**
 * @brief Détermine le prochain coup selon la difficulté
 * 
 * @param game État du jeu
 * @param ai Configuration de l'IA
 * @return Position choisie
 */
static int ai_get_move(game_t* game, const ai_t* ai) {
    switch (ai->difficulty) {
        case EASY:
            return find_random_empty_cell(game);
            
        case MEDIUM:
            return find_medium_move(game);
            
        case HARD:
            return find_best_move(game);
            
        default:
            return find_random_empty_cell(game);
    }
}

/*********************************
 * Implémentation des fonctions publiques
 *********************************/

void init_ai(ai_t* ai, AIDifficulty difficulty) {
    ai->difficulty = difficulty;
    srand((unsigned int)time(NULL));
}

void ai_make_move(game_t* game, ai_t* ai) {
    int move = ai_get_move(game, ai);
    
    if (move != -1) {
        int row = move / N;
        int col = move % N;
        click_on_cell(game, row, col);
    }
}