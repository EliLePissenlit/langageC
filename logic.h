// logic.h
#ifndef LOGIC_H_
#define LOGIC_H_

#include "./game.h"

// Change le joueur actif
void switch_player(game_t *game);

// Gère le clic sur une cellule du plateau
void click_on_cell(game_t *game, int row, int column);

// Réinitialise l'état du jeu
void reset_game(game_t *game);

// Gère le tour d'un joueur
void player_turn(game_t *game, int row, int column);

// Vérifie si un joueur a gagné
int check_player_won(const game_t *game, int player);

// Compte le nombre de cellules d'un certain type
int count_cells(const int *board, int cell);

#endif  // LOGIC_H_