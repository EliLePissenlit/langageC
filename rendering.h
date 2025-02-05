/**
 * @file rendering.h
 * @brief Interface pour le rendu graphique du jeu
 *
 * Ce module gère tous les aspects graphiques du jeu :
 * - Rendu de la grille de jeu
 * - Dessin des symboles (X et O)
 * - Affichage des menus et transitions
 * - États de victoire et animations
 */

#ifndef RENDERING_H_
#define RENDERING_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./game.h"

/*********************************
 * Configuration du rendu
 *********************************/

/** @brief Épaisseur des traits pour les symboles */
#define SYMBOL_THICKNESS 10

/** @brief Ratio de taille des symboles par rapport aux cellules */
#define SYMBOL_SIZE_RATIO 0.25f

/*********************************
 * Types et structures
 *********************************/

/** @brief Couleurs utilisées dans le jeu */
typedef struct {
    SDL_Color grid;           /**< Couleur de la grille */
    SDL_Color player_x;       /**< Couleur du joueur X */
    SDL_Color player_o;       /**< Couleur du joueur O */
    SDL_Color tie;           /**< Couleur pour match nul */
    SDL_Color button;        /**< Couleur des boutons */
    SDL_Color text;          /**< Couleur du texte */
    SDL_Color overlay;       /**< Couleur de fond semi-transparente */
} GameColors;

/*********************************
 * Fonctions de rendu principales
 *********************************/

/**
 * @brief Fonction principale de rendu du jeu
 * 
 * Gère le rendu de tous les éléments selon l'état du jeu :
 * - Partie en cours
 * - État de victoire
 * - Menu de fin de partie
 *
 * @param renderer Contexte de rendu SDL
 * @param game État du jeu à afficher
 * @param font Police pour le texte
 */
void render_game(SDL_Renderer* renderer, const game_t* game, TTF_Font* font);

/**
 * @brief Affiche le menu de victoire
 *
 * Affiche :
 * - Message de victoire/défaite
 * - Bouton pour rejouer
 * - Bouton pour retourner au menu
 * 
 * @param renderer Contexte de rendu SDL
 * @param game État du jeu
 * @param font Police pour le texte
 */
void render_victory_menu(SDL_Renderer* renderer, const game_t* game, TTF_Font* font);

#endif  /* RENDERING_H_ */