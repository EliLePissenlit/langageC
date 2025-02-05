/**
 * @file logic.c
 * @brief Implémentation de la logique métier du jeu Tic-Tac-Toe
 */

#include "./game.h"
#include "./logic.h"

/*********************************
 * Fonctions statiques internes
 *********************************/

/**
 * @brief Vérifie les conditions de fin de partie
 * 
 * Détermine si :
 * - Un joueur a gagné
 * - Il y a match nul
 * - La partie continue
 *
 * @param game Pointeur vers l'état du jeu
 */
static void check_game_over_condition(game_t* game) {
    // Vérifie d'abord si X a gagné
    if (check_player_won(game, PLAYER_X)) {
        game->state = PLAYER_X_WON_STATE;
        return;
    }
    
    // Puis si O a gagné
    if (check_player_won(game, PLAYER_O)) {
        game->state = PLAYER_O_WON_STATE;
        return;
    }
    
    // Enfin, vérifie s'il y a match nul
    if (count_cells(game->board, EMPTY) == 0) {
        game->state = TIE_STATE;
    }
}

/**
 * @brief Vérifie si une position est valide pour jouer
 * @param game État du jeu
 * @param row Ligne
 * @param col Colonne
 * @return CHECK_TRUE si la position est valide, CHECK_FALSE sinon
 */
static CheckResult is_valid_move(const game_t* game, int row, int col) {
    // Vérifie que les coordonnées sont dans la grille
    if (!IS_VALID_CELL(row, col)) {
        return CHECK_FALSE;
    }
    
    // Vérifie que la case est vide
    if (game->board[BOARD_INDEX(row, col)] != EMPTY) {
        return CHECK_FALSE;
    }
    
    return CHECK_TRUE;
}

/*********************************
 * Implémentation des fonctions publiques
 *********************************/

void switch_player(game_t* game) {
    game->player = (game->player == PLAYER_X) ? PLAYER_O : PLAYER_X;
}

void player_turn(game_t* game, int row, int column) {
    // Vérifie si le coup est valide
    if (!is_valid_move(game, row, column)) {
        return;
    }
    
    // Place le symbole du joueur actuel
    game->board[BOARD_INDEX(row, column)] = game->player;
    
    // Change de joueur et vérifie la fin de partie
    switch_player(game);
    check_game_over_condition(game);
}

void click_on_cell(game_t* game, int row, int column) {
    // Ne fait rien si le jeu n'est pas en cours
    if (game->state != RUNNING_STATE) {
        return;
    }
    
    player_turn(game, row, column);
}

void reset_game(game_t* game) {
    // Réinitialise l'état du jeu
    game->player = PLAYER_X;  // X commence toujours
    game->state = RUNNING_STATE;
    game->victory_time = 0;
    
    // En mode IA, désactive le mode snake
    if (game->is_ai_mode) {
        game->is_snake_mode = 0;
    }
    
    // Vide toutes les cases
    for (int i = 0; i < N * N; ++i) {
        game->board[i] = EMPTY;
    }
}

CheckResult check_player_won(const game_t* game, int player) {
    int row_count;     // Compteur pour les lignes
    int column_count;  // Compteur pour les colonnes
    int diag1_count;   // Compteur pour la diagonale principale
    int diag2_count;   // Compteur pour la diagonale secondaire

    // Réinitialise les compteurs de diagonales
    diag1_count = 0;
    diag2_count = 0;

    // Vérifie les lignes et colonnes
    for (int i = 0; i < N; ++i) {
        // Réinitialise les compteurs pour chaque ligne/colonne
        row_count = 0;
        column_count = 0;
        
        for (int j = 0; j < N; ++j) {
            // Vérifie la ligne i
            if (game->board[BOARD_INDEX(i, j)] == player) {
                row_count++;
            }

            // Vérifie la colonne i
            if (game->board[BOARD_INDEX(j, i)] == player) {
                column_count++;
            }
        }

        // Si une ligne ou colonne est complète
        if (row_count >= N || column_count >= N) {
            return CHECK_TRUE;
        }

        // Vérifie les diagonales
        if (game->board[BOARD_INDEX(i, i)] == player) {
            diag1_count++;
        }

        if (game->board[BOARD_INDEX(i, N - i - 1)] == player) {
            diag2_count++;
        }
    }

    // Retourne vrai si une diagonale est complète
    return (diag1_count >= N || diag2_count >= N) ? CHECK_TRUE : CHECK_FALSE;
}

int count_cells(const int* board, int cell_type) {
    int count = 0;

    // Parcourt toutes les cellules
    for (int i = 0; i < N * N; ++i) {
        if (board[i] == cell_type) {
            count++;
        }
    }

    return count;
}