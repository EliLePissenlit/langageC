#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>

// Constantes de configuration du jeu
#define N 3                     // Taille de la grille (3x3)
#define SCREEN_WIDTH 640.0      // Largeur de la fenêtre
#define SCREEN_HEIGHT 480.0     // Hauteur de la fenêtre
#define CELL_WIDTH (SCREEN_WIDTH / N)   // Largeur d'une cellule
#define CELL_HEIGHT (SCREEN_HEIGHT / N)  // Hauteur d'une cellule

// Temps d'affichage de l'écran de victoire (en millisecondes)
#define VICTORY_DISPLAY_TIME 1000

// États possibles des cellules
#define EMPTY 0        // Case vide
#define PLAYER_X 1     // Case occupée par X
#define PLAYER_O 2     // Case occupée par O

// États possibles du jeu
#define RUNNING_STATE 0         // Partie en cours
#define PLAYER_X_WON_STATE 1    // Victoire du joueur X
#define PLAYER_O_WON_STATE 2    // Victoire du joueur O
#define TIE_STATE 3            // Match nul
#define VICTORY_MENU_STATE 4    // Menu de fin de partie
#define QUIT_STATE 5           // Quitter le jeu

// Structure principale du jeu
typedef struct {
    int board[N * N];          // Plateau de jeu
    int player;               // Joueur courant
    int state;               // État actuel du jeu
    Uint32 victory_time;     // Timestamp de la victoire
    SDL_Rect replay_button;  // Bouton "Rejouer"
    SDL_Rect menu_button;    // Bouton "Menu principal"
    int is_snake_mode;       // Flag pour indiquer si on est en mode snake
    int is_ai_mode;         // Flag pour indiquer si on est en mode IA
} game_t;

#endif  // GAME_H_