// logic.c
#include "./game.h"
#include "./logic.h"

// Change le joueur actif
void switch_player(game_t *game)
{
    if (game->player == PLAYER_X) {
        game->player = PLAYER_O;
    } else if (game->player == PLAYER_O) {
        game->player = PLAYER_X;
    }
}

// Vérifie si un joueur a gagné en vérifiant toutes les conditions de victoire
int check_player_won(const game_t *game, int player)
{
    int row_count = 0;     // Compteur pour les lignes
    int column_count = 0;  // Compteur pour les colonnes
    int diag1_count = 0;   // Compteur pour la diagonale principale
    int diag2_count = 0;   // Compteur pour la diagonale secondaire

    // Vérifie les lignes et colonnes
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            // Vérifie la ligne i
            if (game->board[i * N + j] == player) {
                row_count++;
            }

            // Vérifie la colonne i
            if (game->board[j * N + i] == player) {
                column_count++;
            }
        }

        // Si une ligne ou colonne est complète
        if (row_count >= N || column_count >= N) {
            return 1;
        }

        row_count = 0;
        column_count = 0;

        // Vérifie les diagonales
        if (game->board[i * N + i] == player) {
            diag1_count++;
        }

        if (game->board[i * N + N - i - 1] == player) {
            diag2_count++;
        }
    }

    // Retourne 1 si une diagonale est complète
    return diag1_count >= N || diag2_count >= N;
}

// Compte le nombre de cellules d'un certain type sur le plateau
int count_cells(const int *board, int cell)
{
    int count = 0;

    for (int i = 0; i < N * N; ++i) {
        if (board[i] == cell) {
            count++;
        }
    }

    return count;
}

// Vérifie les conditions de fin de partie
void game_over_condition(game_t *game)
{
    if (check_player_won(game, PLAYER_X)) {
        game->state = PLAYER_X_WON_STATE;
    } else if (check_player_won(game, PLAYER_O)) {
        game->state = PLAYER_O_WON_STATE;
    } else if (count_cells(game->board, EMPTY) == 0) {
        game->state = TIE_STATE;
    }
}

// Gère le tour d'un joueur
void player_turn(game_t *game, int row, int column)
{
    // Vérifie si la case est vide
    if (game->board[row * N + column] == EMPTY) {
        // Place le symbole du joueur
        game->board[row * N + column] = game->player;
        // Change de joueur
        switch_player(game);
        // Vérifie si la partie est terminée
        game_over_condition(game);
    }
}

// Réinitialise complètement l'état du jeu
void reset_game(game_t *game)
{
    game->player = PLAYER_X;  // X commence toujours
    game->state = RUNNING_STATE;
    game->victory_time = 0;
    
    // Si on est en mode IA, on s'assure que le mode snake est désactivé
    if (game->is_ai_mode) {
        game->is_snake_mode = 0;
    }
    
    // Vide toutes les cases
    for (int i = 0; i < N * N; ++i) {
        game->board[i] = EMPTY;
    }
}

// Gère le clic sur une cellule du plateau
void click_on_cell(game_t *game, int row, int column)
{
    if (game->state == RUNNING_STATE) {
        // Si le jeu est en cours, joue le coup
        player_turn(game, row, column);
    } else {
        // Sinon réinitialise le jeu
        reset_game(game);
    }
}