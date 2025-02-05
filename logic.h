/**
 * @file logic.h
 * @brief Interface pour la logique métier du jeu Tic-Tac-Toe
 *
 * Ce module gère toutes les règles du jeu, incluant :
 * - La manipulation du plateau de jeu
 * - La gestion des tours de jeu
 * - La vérification des conditions de victoire
 * - Le comptage des cellules
 */

#ifndef LOGIC_H_
#define LOGIC_H_

#include "./game.h"

/**
 * @brief Type de retour pour les fonctions de vérification
 * Utilisé pour une meilleure lisibilité du code
 */
typedef enum {
    CHECK_FALSE = 0,
    CHECK_TRUE = 1
} CheckResult;

/*********************************
 * Gestion des tours
 *********************************/

/**
 * @brief Change le joueur actif
 * Alterne entre PLAYER_X et PLAYER_O
 * @param game Pointeur vers l'état du jeu
 */
void switch_player(game_t* game);

/**
 * @brief Gère le clic sur une cellule du plateau
 * 
 * Cette fonction est le point d'entrée principal pour jouer un coup.
 * Elle vérifie la validité du coup et met à jour l'état du jeu en conséquence.
 *
 * @param game Pointeur vers l'état du jeu
 * @param row Ligne ciblée (0-2)
 * @param column Colonne ciblée (0-2)
 */
void click_on_cell(game_t* game, int row, int column);

/*********************************
 * Gestion du plateau
 *********************************/

/**
 * @brief Réinitialise complètement l'état du jeu
 * 
 * Remet à zéro :
 * - Le plateau de jeu
 * - Le joueur actif
 * - L'état du jeu
 * - Le timer de victoire
 *
 * @param game Pointeur vers l'état du jeu
 */
void reset_game(game_t* game);

/**
 * @brief Gère un tour de jeu
 * 
 * Place un symbole sur le plateau si possible et vérifie
 * les conditions de fin de partie.
 *
 * @param game Pointeur vers l'état du jeu
 * @param row Ligne ciblée
 * @param column Colonne ciblée
 */
void player_turn(game_t* game, int row, int column);

/*********************************
 * Vérifications
 *********************************/

/**
 * @brief Vérifie si un joueur a gagné
 *
 * Vérifie toutes les conditions de victoire possibles :
 * - Lignes complètes
 * - Colonnes complètes
 * - Diagonales complètes
 *
 * @param game Pointeur vers l'état du jeu
 * @param player Joueur à vérifier (PLAYER_X ou PLAYER_O)
 * @return CHECK_TRUE si le joueur a gagné, CHECK_FALSE sinon
 */
CheckResult check_player_won(const game_t* game, int player);

/**
 * @brief Compte le nombre de cellules d'un certain type
 *
 * Utile pour :
 * - Vérifier s'il reste des cases vides
 * - Compter les symboles d'un joueur
 * - Détecter un match nul
 *
 * @param board Plateau de jeu
 * @param cell_type Type de cellule à compter (EMPTY, PLAYER_X, ou PLAYER_O)
 * @return Nombre de cellules du type spécifié
 */
int count_cells(const int* board, int cell_type);

#endif  /* LOGIC_H_ */