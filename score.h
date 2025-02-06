/**
 * @file score.h
 * @brief Interface pour la gestion des scores
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*********************************
 * Configuration des scores
 *********************************/
#define MAX_USERNAME_LENGTH 32
#define MAX_USERNAME_INPUT_LENGTH 16
#define SCORES_FILE "scores.txt"
#define MAX_SCORES 100

typedef enum {
    USERNAME_INPUT_STATE,
    GAME_RUNNING_STATE
} ScoreState;

/*********************************
 * Structures
 *********************************/

/**
 * @brief Structure pour stocker les scores d'une session
 */
typedef struct {
    char username[MAX_USERNAME_LENGTH]; /** Nom du joueur */
    int wins;                          /** Nombre de victoires */
    int games_played;                  /** Nombre total de parties */
} session_score_t;

/**
 * @brief Structure pour stocker tous les scores historiques
 */
typedef struct {
    session_score_t scores[MAX_SCORES]; /** Tableau des scores */
    int count;                         /** Nombre de scores enregistrés */
} score_history_t;

// Forward declaration de game_t
struct game_t;

/*********************************
 * Fonctions publiques
 *********************************/

/**
 * @brief Initialise le système de score
 * 
 * Charge les scores depuis le fichier et demande le pseudo
 *
 * @param game État du jeu
 * @return 0 si succès, -1 si erreur
 */
int init_score_system(struct game_t* game);

/**
 * @brief Met à jour le score du joueur actuel
 * 
 * @param game État du jeu
 * @param won 1 si victoire, 0 si défaite/égalité
 */
void update_player_score(struct game_t* game, int won);

/**
 * @brief Sauvegarde les scores dans le fichier
 * 
 * @param game État du jeu
 */
void save_scores(const struct game_t* game);

/**
 * @brief Gère les événements de saisie du pseudo
 * 
 * @param game État du jeu
 * @param event Événement SDL à traiter
 */
void handle_username_input(struct game_t* game, SDL_Event* event);

/**
 * @brief Affiche l'interface de saisie du pseudo
 * 
 * @param renderer Contexte de rendu SDL
 * @param game État du jeu
 * @param font Police pour le texte
 */
void render_username_input(SDL_Renderer* renderer, struct game_t* game, TTF_Font* font);

/**
 * @brief Nettoie le système de score
 * 
 * @param game État du jeu
 */
void cleanup_score_system(struct game_t* game);

/**
 * @brief Affiche le tableau des scores
 * 
 * @param renderer Contexte de rendu SDL
 * @param game État du jeu
 * @param font Police pour le texte
 */
void render_scoreboard(SDL_Renderer* renderer, const struct game_t* game, TTF_Font* font);

#endif /* SCORE_H_ */