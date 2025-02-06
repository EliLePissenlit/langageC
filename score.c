#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "./game.h"
#include "./score.h"

/*********************************
 * Configuration des styles
 *********************************/
#define INPUT_BOX_PADDING 20
#define INPUT_BOX_BORDER_RADIUS 10

// Chemin du fichier de scores
static const char* SCORES_FILENAME = "scores.txt";

static const SDL_Color TEXT_COLOR = {255, 255, 255, 255};
static const SDL_Color BOX_COLOR = {40, 44, 52, 220};
static const SDL_Color BORDER_COLOR = {97, 175, 239, 255};
static const SDL_Color PLACEHOLDER_COLOR = {150, 150, 150, 255};
static const SDL_Color HEADER_COLOR = {97, 175, 239, 255};

/*********************************
 * Fonctions utilitaires statiques
 *********************************/

/**
 * @brief Charge les scores depuis le fichier
 */
static int load_scores(score_history_t* history) {
    FILE* file = fopen(SCORES_FILENAME, "r");
    if (!file) {
        // Tente de créer le fichier s'il n'existe pas
        file = fopen(SCORES_FILENAME, "w");
        if (file) {
            fclose(file);
            history->count = 0;
            return 0;
        }
        return -1;
    }

    history->count = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), file) && history->count < MAX_SCORES) {
        session_score_t* score = &history->scores[history->count];
        if (sscanf(line, "%s %d %d", 
                   score->username, 
                   &score->wins, 
                   &score->games_played) == 3) {
            history->count++;
        }
    }

    fclose(file);
    return 1;
}

/**
 * @brief Sauvegarde les scores dans le fichier
 */
static void write_scores(const score_history_t* history) {
    FILE* file = fopen(SCORES_FILENAME, "w");
    if (!file) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s en écriture\n", SCORES_FILENAME);
        return;
    }

    for (int i = 0; i < history->count; i++) {
        fprintf(file, "%s %d %d\n", 
                history->scores[i].username,
                history->scores[i].wins,
                history->scores[i].games_played);
    }

    fclose(file);
}

/**
 * @brief Dessine une boîte avec bordure arrondie
 */
static void draw_input_box(SDL_Renderer* renderer, const SDL_Rect* box) {
    // Fond
    roundedBoxRGBA(renderer,
        box->x, box->y, box->x + box->w, box->y + box->h,
        INPUT_BOX_BORDER_RADIUS,
        BOX_COLOR.r, BOX_COLOR.g, BOX_COLOR.b, BOX_COLOR.a);
    
    // Bordure
    roundedRectangleRGBA(renderer,
        box->x, box->y, box->x + box->w, box->y + box->h,
        INPUT_BOX_BORDER_RADIUS,
        BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
}

/*********************************
 * Implémentation des fonctions publiques
 *********************************/

int init_score_system(game_t* game) {
    game->score_history = malloc(sizeof(score_history_t));
    if (!game->score_history) return -1;

    game->score_history->count = 0;
    
    // Charge les scores existants
    int result = load_scores(game->score_history);
    if (result < 0) {
        fprintf(stderr, "Erreur: Impossible d'initialiser le système de scores\n");
        free(game->score_history);
        return -1;
    }

    // Initialise l'état et le buffer de saisie
    game->score_state = USERNAME_INPUT_STATE;
    memset(game->username_buffer, 0, MAX_USERNAME_INPUT_LENGTH);
    
    // Initialise le score de la session
    memset(&game->current_score, 0, sizeof(session_score_t));

    return 0;
}

void update_player_score(game_t* game, int won) {
    game->current_score.games_played++;
    if (won) game->current_score.wins++;

    // Met à jour ou ajoute le score dans l'historique
    int found = 0;
    for (int i = 0; i < game->score_history->count; i++) {
        if (strcmp(game->score_history->scores[i].username, 
                  game->current_score.username) == 0) {
            game->score_history->scores[i] = game->current_score;
            found = 1;
            break;
        }
    }

    if (!found && game->score_history->count < MAX_SCORES) {
        game->score_history->scores[game->score_history->count++] = 
            game->current_score;
    }

    // Sauvegarde immédiatement les changements
    write_scores(game->score_history);
}

void render_username_input(SDL_Renderer* renderer, game_t* game, TTF_Font* font) {
    // Dimensions de la boîte de saisie
    SDL_Rect input_box = {
        game->dimensions.window_width / 4,
        game->dimensions.window_height / 3,
        game->dimensions.window_width / 2,
        80
    };

    // Dessine la boîte de saisie
    draw_input_box(renderer, &input_box);

    // Affiche le titre
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, 
        "Entrez votre pseudo :", TEXT_COLOR);
    if (title_surface) {
        SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
        SDL_Rect title_rect = {
            game->dimensions.window_width / 2 - title_surface->w / 2,
            input_box.y - 40,
            title_surface->w,
            title_surface->h
        };
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_DestroyTexture(title_texture);
        SDL_FreeSurface(title_surface);
    }

    // Affiche le texte saisi ou un placeholder
    const char* display_text = strlen(game->username_buffer) > 0 ? 
        game->username_buffer : "...";
    SDL_Color color = strlen(game->username_buffer) > 0 ? 
        TEXT_COLOR : PLACEHOLDER_COLOR;

    SDL_Surface* text_surface = TTF_RenderText_Blended(font, display_text, color);
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = {
            input_box.x + INPUT_BOX_PADDING,
            input_box.y + (input_box.h - text_surface->h) / 2,
            text_surface->w,
            text_surface->h
        };
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

void handle_username_input(game_t* game, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_RETURN && 
            strlen(game->username_buffer) > 0) {
            // Copie le pseudo dans la structure de score
            strncpy(game->current_score.username, game->username_buffer, 
                    MAX_USERNAME_LENGTH - 1);
            game->current_score.username[MAX_USERNAME_LENGTH - 1] = '\0';
            game->score_state = GAME_RUNNING_STATE;
        }
        else if (event->key.keysym.sym == SDLK_BACKSPACE && 
                 strlen(game->username_buffer) > 0) {
            // Supprime le dernier caractère
            game->username_buffer[strlen(game->username_buffer) - 1] = '\0';
        }
    }
    else if (event->type == SDL_TEXTINPUT) {
        // Ajoute le caractère si on n'a pas atteint la limite
        if (strlen(game->username_buffer) < MAX_USERNAME_INPUT_LENGTH - 1) {
            strcat(game->username_buffer, event->text.text);
        }
    }
}

void render_scoreboard(SDL_Renderer* renderer, const game_t* game, TTF_Font* font) {
    // Fond avec dégradé subtil
    for (int y = 0; y < game->dimensions.window_height; y++) {
        float factor = (float)y / game->dimensions.window_height;
        SDL_SetRenderDrawColor(renderer,
            18 + (int)(10 * factor),
            18 + (int)(10 * factor),
            24 + (int)(10 * factor),
            255);
        SDL_RenderDrawLine(renderer, 0, y, game->dimensions.window_width, y);
    }

    int y_pos = game->dimensions.window_height * 0.1; // 10% du haut

    // Titre avec lignes décoratives
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, 
        "Tableau des Scores", TEXT_COLOR);
    if (title_surface) {
        // Lignes décoratives
        int line_length = game->dimensions.window_width * 0.2; // 20% de la largeur
        SDL_SetRenderDrawColor(renderer, 220, 40, 40, 255);  // Rouge
        SDL_RenderDrawLine(renderer,
            game->dimensions.window_width/2 - line_length, y_pos + title_surface->h/2,
            game->dimensions.window_width/2 - line_length/4, y_pos + title_surface->h/2);
        
        SDL_SetRenderDrawColor(renderer, 40, 100, 220, 255);  // Bleu
        SDL_RenderDrawLine(renderer,
            game->dimensions.window_width/2 + line_length/4, y_pos + title_surface->h/2,
            game->dimensions.window_width/2 + line_length, y_pos + title_surface->h/2);

        // Titre
        SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
        SDL_Rect title_rect = {
            (game->dimensions.window_width - title_surface->w) / 2,
            y_pos,
            title_surface->w,
            title_surface->h
        };
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_DestroyTexture(title_texture);
        SDL_FreeSurface(title_surface);
    }
    
    y_pos += game->dimensions.window_height * 0.1; // Espace après le titre

    // Calcul des positions relatives des colonnes
    const float margin = game->dimensions.window_width * 0.05; // 5% de marge
    const float usable_width = game->dimensions.window_width - (2 * margin);
    
    // Distribution des colonnes (en pourcentage de l'espace utilisable)
    const int col_joueur = margin;
    const int col_victoires = margin + (usable_width * 0.4);  // 40% pour le nom
    const int col_parties = margin + (usable_width * 0.6);    // 20% pour les victoires
    const int col_ratio = margin + (usable_width * 0.8);      // 20% pour les parties

    // En-tête avec colonnes alignées
    SDL_Surface* header_joueur = TTF_RenderText_Blended(font, "Joueur", HEADER_COLOR);
    SDL_Surface* header_victoires = TTF_RenderText_Blended(font, "Victoires", HEADER_COLOR);
    SDL_Surface* header_parties = TTF_RenderText_Blended(font, "Parties", HEADER_COLOR);
    SDL_Surface* header_ratio = TTF_RenderText_Blended(font, "Ratio", HEADER_COLOR);

    if (header_joueur && header_victoires && header_parties && header_ratio) {
        // Affichage des en-têtes
        SDL_Texture* texture;
        SDL_Rect rect;

        // Joueur
        texture = SDL_CreateTextureFromSurface(renderer, header_joueur);
        rect = (SDL_Rect){col_joueur, y_pos, header_joueur->w, header_joueur->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);

        // Victoires
        texture = SDL_CreateTextureFromSurface(renderer, header_victoires);
        rect = (SDL_Rect){col_victoires, y_pos, header_victoires->w, header_victoires->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);

        // Parties
        texture = SDL_CreateTextureFromSurface(renderer, header_parties);
        rect = (SDL_Rect){col_parties, y_pos, header_parties->w, header_parties->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);

        // Ratio
        texture = SDL_CreateTextureFromSurface(renderer, header_ratio);
        rect = (SDL_Rect){col_ratio, y_pos, header_ratio->w, header_ratio->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    // Libération des surfaces d'en-tête
    SDL_FreeSurface(header_joueur);
    SDL_FreeSurface(header_victoires);
    SDL_FreeSurface(header_parties);
    SDL_FreeSurface(header_ratio);
    
    y_pos += game->dimensions.window_height * 0.05; // Espace après les en-têtes

    // Liste des scores avec colonnes alignées
    for (int i = 0; i < game->score_history->count; i++) {
        const session_score_t* score = &game->score_history->scores[i];
        
        // Alterne l'opacité pour une meilleure lisibilité
        SDL_Color row_color = TEXT_COLOR;
        row_color.a = (i % 2 == 0) ? 255 : 200;

        // Prépare chaque colonne séparément
        char ratio_text[32];
        float ratio = score->games_played > 0 ? 
            (float)score->wins / score->games_played * 100.0f : 0.0f;
        snprintf(ratio_text, sizeof(ratio_text), "%.1f%%", ratio);

        char victoires_text[32];
        snprintf(victoires_text, sizeof(victoires_text), "%d", score->wins);

        char parties_text[32];
        snprintf(parties_text, sizeof(parties_text), "%d", score->games_played);

        // Crée et affiche chaque colonne
        SDL_Surface* surfaces[4];
        surfaces[0] = TTF_RenderText_Blended(font, score->username, row_color);
        surfaces[1] = TTF_RenderText_Blended(font, victoires_text, row_color);
        surfaces[2] = TTF_RenderText_Blended(font, parties_text, row_color);
        surfaces[3] = TTF_RenderText_Blended(font, ratio_text, row_color);

        int columns[4] = {col_joueur, col_victoires, col_parties, col_ratio};

        for (int j = 0; j < 4; j++) {
            if (surfaces[j]) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaces[j]);
                SDL_Rect rect = {
                    columns[j],
                    y_pos,
                    surfaces[j]->w,
                    surfaces[j]->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surfaces[j]);
            }
        }

        y_pos += game->dimensions.window_height * 0.04; // Espacement entre les lignes proportionnel
    }
}
void cleanup_score_system(game_t* game) {
    if (game->score_history) {
        write_scores(game->score_history);  // Sauvegarde finale
        free(game->score_history);
        game->score_history = NULL;
    }
}