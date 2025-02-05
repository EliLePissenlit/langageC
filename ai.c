// ai.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "./ai.h"
#include "./logic.h"

// Fonctions utilitaires pour l'algorithme minimax
static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

// Initialisation de l'IA avec un niveau de difficulté
void init_ai(ai_t *ai, AIDifficulty difficulty) {
    printf("Initializing AI with difficulty: %d\n", difficulty);
    ai->difficulty = difficulty;
    srand(time(NULL));  // Initialise le générateur de nombres aléatoires
}

// Vérifie si un coup est gagnant pour un joueur donné
int is_winning_move(game_t *game, int position, int player) {
    int original = game->board[position];
    game->board[position] = player;
    int result = check_player_won(game, player);
    game->board[position] = original;
    return result;
}

// Trouve une case vide aléatoire pour le mode facile
int find_empty_cell(game_t *game) {
    int empty_cells[N * N];
    int count = 0;

    // Collecte toutes les cases vides
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            empty_cells[count++] = i;
        }
    }

    // Retourne une case vide aléatoire
    return (count > 0) ? empty_cells[rand() % count] : -1;
}

// Vérifie si le jeu est terminé (victoire ou match nul)
int is_game_over(game_t *game) {
    return check_player_won(game, PLAYER_X) || 
           check_player_won(game, PLAYER_O) || 
           count_cells(game->board, EMPTY) == 0;
}

// Évalue la position actuelle pour l'algorithme minimax
int evaluate_board(game_t *game) {
    if (check_player_won(game, PLAYER_O)) return 10;    // L'IA gagne
    if (check_player_won(game, PLAYER_X)) return -10;   // Le joueur gagne
    return 0;  // Match nul ou partie en cours
}

// Algorithme Minimax pour le mode difficile
// Retourne le meilleur score possible pour le joueur courant
int minimax(game_t *game, int depth, int is_maximizing) {
    int score = evaluate_board(game);

    // Cas de base : victoire/défaite ou match nul
    if (score == 10 || score == -10) return score;
    if (count_cells(game->board, EMPTY) == 0) return 0;

    if (is_maximizing) {
        // Tour de l'IA
        int best = -1000;
        for (int i = 0; i < N * N; i++) {
            if (game->board[i] == EMPTY) {
                game->board[i] = PLAYER_O;
                best = max(best, minimax(game, depth + 1, !is_maximizing));
                game->board[i] = EMPTY;
            }
        }
        return best;
    } else {
        // Tour du joueur
        int best = 1000;
        for (int i = 0; i < N * N; i++) {
            if (game->board[i] == EMPTY) {
                game->board[i] = PLAYER_X;
                best = min(best, minimax(game, depth + 1, !is_maximizing));
                game->board[i] = EMPTY;
            }
        }
        return best;
    }
}

// Trouve le meilleur coup possible pour le mode difficile
int find_best_move(game_t *game) {
    int best_val = -1000;
    int best_move = -1;

    // Teste chaque coup possible
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            game->board[i] = PLAYER_O;
            int move_val = minimax(game, 0, 0);
            game->board[i] = EMPTY;

            if (move_val > best_val) {
                best_move = i;
                best_val = move_val;
            }
        }
    }

    return best_move;
}

// Trouve un coup pour le niveau moyen
// Combine stratégie simple et aléatoire
int find_medium_move(game_t *game) {
    printf("Starting medium move calculation\n");

    // 1. Vérifie d'abord si l'IA peut gagner
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            if (is_winning_move(game, i, PLAYER_O)) {
                return i;
            }
        }
    }

    // 2. Bloque le joueur s'il peut gagner
    for (int i = 0; i < N * N; i++) {
        if (game->board[i] == EMPTY) {
            if (is_winning_move(game, i, PLAYER_X)) {
                return i;
            }
        }
    }

    // 3. Si aucune stratégie évidente, joue aléatoirement
    return find_empty_cell(game);
}

// Détermine le prochain coup de l'IA selon sa difficulté
int ai_get_move(game_t *game, ai_t *ai) {
    printf("AI move with difficulty: %d\n", ai->difficulty);
    int move = -1;

    switch (ai->difficulty) {
        case EASY:   // Coups aléatoires
            move = find_empty_cell(game);
            break;
            
        case MEDIUM: // Stratégie simple + aléatoire
            move = find_medium_move(game);
            break;
            
        case HARD:   // Utilise minimax
            move = find_best_move(game);
            break;
            
        default:     // Par défaut, joue aléatoirement
            move = find_empty_cell(game);
            break;
    }

    return move;
}

// Exécute le coup de l'IA
void ai_make_move(game_t *game, ai_t *ai) {
    printf("\n=== AI MAKING MOVE ===\n");
    
    int move = ai_get_move(game, ai);
    
    if (move != -1) {
        int row = move / N;
        int column = move % N;
        
        printf("Converting to row=%d, col=%d\n", row, column);
        click_on_cell(game, row, column);
    }
    printf("=== END AI MOVE ===\n\n");
}