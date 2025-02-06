/**
 * @file main.c
 * @brief Point d'entrée principal du jeu Tic-Tac-Toe avec modes classique et Snake
 * 
 * Ce fichier contient la boucle principale du jeu et gère :
 * - L'initialisation des composants SDL2
 * - La création et gestion de la fenêtre
 * - La boucle événementielle principale
 * - La gestion des différents états du jeu
 * - Le nettoyage des ressources
 */

/*********************************
 * Includes
 *********************************/
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "./game.h"
#include "./logic.h"
#include "./rendering.h"
#include "./menu.h"
#include "./ai.h"
#include "./snake.h"
#include "./window.h"

/* Constantes locales */
#define DEFAULT_WINDOW_POS_X 100
#define DEFAULT_WINDOW_POS_Y 100
#define DEFAULT_FONT_SIZE 24
#define DEFAULT_FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

/*********************************
 * Prototypes des fonctions statiques
 *********************************/
static void handle_snake_mode_turn(game_t* game, SDL_Renderer* renderer,
                                 int row, int col, int is_ai_game, ai_t* ai);
static void handle_classic_mode_turn(game_t* game, int row, int col, 
                                   int is_ai_game, ai_t* ai);
static void handle_victory_transition(game_t* game);
static void handle_menu_click_result(menu_t* menu, game_t* game, ai_t* ai,
                                   int* is_ai_game, int x, int y);

/*********************************
 * Fonctions d'initialisation
 *********************************/
/**
 * @brief Initialise SDL2 et ses composants
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
static int initialize_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL2: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    return 0;
}

/**
 * @brief Crée la fenêtre du jeu
 * @return Pointeur vers la fenêtre créée, NULL en cas d'erreur
 */
static SDL_Window* create_window(void) {
    SDL_Window* window = SDL_CreateWindow(
        "Tic-Tac-Toe",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        INITIAL_WIDTH,
        INITIAL_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        fprintf(stderr, "Erreur création fenêtre: %s\n", SDL_GetError());
    }

    return window;
}

/**
 * @brief Crée le renderer avec les options optimales
 * @param window Fenêtre SDL associée
 * @return Pointeur vers le renderer créé, NULL en cas d'erreur
 */
static SDL_Renderer* create_renderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == NULL) {
        fprintf(stderr, "Erreur création renderer: %s\n", SDL_GetError());
    }

    return renderer;
}

/**
 * @brief Initialise la structure principale du jeu
 * @param game Structure à initialiser
 */
static void initialize_game_state(game_t* game) {
    // Initialisation du plateau
    for (int i = 0; i < N * N; i++) {
        game->board[i] = EMPTY;
    }

    // Configuration initiale
    game->player = PLAYER_X;
    game->state = RUNNING_STATE;
    game->victory_time = 0;
    game->is_snake_mode = 0;
    game->is_fullscreen = 0;

    // Initialisation des dimensions
    update_window_dimensions(game, INITIAL_WIDTH, INITIAL_HEIGHT);
}

/*********************************
 * Fonctions de gestion
 *********************************/
/**
 * @brief Gère les événements de clic pendant le jeu
 * @param game État du jeu
 * @param renderer Renderer SDL
 * @param event Événement SDL
 * @param is_ai_game Indique si on joue contre l'IA
 * @param ai Configuration de l'IA
 */
static void handle_game_click(game_t* game, SDL_Renderer* renderer, 
                            SDL_Event* event, int is_ai_game, ai_t* ai, menu_t* menu) {
    // Menu de victoire
    if (game->state == VICTORY_MENU_STATE) {
        SDL_Point click = {event->button.x, event->button.y};
        
        if (SDL_PointInRect(&click, &game->replay_button)) {
            reset_game(game);
            game->victory_time = 0;
        } else if (SDL_PointInRect(&click, &game->menu_button)) {
            reset_game(game);
            game->victory_time = 0;
            menu->mode = MENU_STATE;
            return;
        }
        return;
    }

    // Jeu en cours
    if (game->state == RUNNING_STATE) {
        int row = event->button.y / game->dimensions.cell_height;
        int col = event->button.x / game->dimensions.cell_width;
        
        // Vérifie si la case est libre
        if (game->board[row * N + col] == EMPTY) {
            if (game->is_snake_mode) {
                handle_snake_mode_turn(game, renderer, row, col, is_ai_game, ai);
            } else {
                handle_classic_mode_turn(game, row, col, is_ai_game, ai);
            }
        }
    }
}

/**
 * @brief Gère un tour en mode Snake
 */
static void handle_snake_mode_turn(game_t* game, SDL_Renderer* renderer, 
                                 int row, int col, int is_ai_game, ai_t* ai) {
    SnakeState result = play_snake_minigame(game, renderer, row, col);
    
    if (result == SNAKE_WON) {
        click_on_cell(game, row, col);
        if (is_ai_game && game->state == RUNNING_STATE && 
            game->player == PLAYER_O) {
            ai_make_move(game, ai);
        }
    } else if (result == SNAKE_LOST) {
        switch_player(game);
        if (is_ai_game && game->player == PLAYER_O) {
            ai_make_move(game, ai);
        }
    }
}

/**
 * @brief Gère un tour en mode classique
 */
static void handle_classic_mode_turn(game_t* game, int row, int col, 
                                   int is_ai_game, ai_t* ai) {
    click_on_cell(game, row, col);
    if (is_ai_game && game->state == RUNNING_STATE && 
        game->player == PLAYER_O) {
        ai_make_move(game, ai);
    }
}

static void handle_victory_transition(game_t* game) {
    if (game->state == PLAYER_X_WON_STATE || 
        game->state == PLAYER_O_WON_STATE || 
        game->state == TIE_STATE) {
        
        // Si nouvelle victoire, enregistre le temps
        if (game->victory_time == 0) {
            game->victory_time = SDL_GetTicks();
        }
        
        // Après le délai, affiche le menu de victoire
        if (SDL_GetTicks() - game->victory_time >= VICTORY_DISPLAY_TIME) {
            game->state = VICTORY_MENU_STATE;
        }
    }
}

static void handle_menu_click_result(menu_t* menu, game_t* game, ai_t* ai,
                                   int* is_ai_game, int x, int y) {
    int result = handle_menu_click(menu, x, y);
    
    if (result == 0) {  // Mode IA sélectionné
        *is_ai_game = 1;
        game->is_snake_mode = 0;
    } else if (result == 1) {  // Mode JcJ sélectionné
        *is_ai_game = 0;
    } else if (result == MODE_CLASSIC) {
        *is_ai_game = 0;
        game->is_snake_mode = 0;
        menu->mode = GAME_STATE;
    } else if (result == MODE_SNAKE) {
        *is_ai_game = 0;
        game->is_snake_mode = 1;
        menu->mode = GAME_STATE;
    } else if (result >= EASY && result <= HARD) {
        init_ai(ai, result);
        *is_ai_game = 1;
        game->is_snake_mode = 0;
        menu->mode = GAME_STATE;
    }
}

/**
 * @brief Fonction principale
 */
int main(int argc, char* argv[]) {
    (void)argc; // Évite l'avertissement de paramètre non utilisé
    (void)argv;

    // Initialisation des composants
    if (initialize_sdl() < 0) {
        return EXIT_FAILURE;
    }

    // Création de la fenêtre et du renderer
    SDL_Window* window = create_window();
    if (!window) {
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = create_renderer(window);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Chargement de la police
    TTF_Font* font = TTF_OpenFont(DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE);
    if (!font) {
        fprintf(stderr, "Erreur chargement police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Initialisation des structures de jeu
    game_t game;
    initialize_game_state(&game);

    menu_t menu;
    ai_t ai;
    int is_ai_game = 0;
    
    init_menu(&menu);

    // Boucle principale
    SDL_Event event;
    while (menu.mode != QUIT_STATE) {
        // Gestion de la transition vers le menu de victoire
        handle_victory_transition(&game);

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                menu.mode = QUIT_STATE;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (menu.mode == MENU_STATE || 
                    menu.mode == AI_DIFFICULTY_STATE || 
                    menu.mode == GAME_MODE_STATE) {
                    handle_menu_click_result(&menu, &game, &ai, 
                                          &is_ai_game, event.button.x, 
                                          event.button.y);
                }
                else if (menu.mode == GAME_STATE) {
                    handle_game_click(&game, renderer, &event, is_ai_game, &ai, &menu);
                }
            }

            // Gestion des événements de fenêtre
            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        // Force une taille minimale
                        int width = event.window.data1;
                        int height = event.window.data2;
                        if (width < MIN_WINDOW_SIZE) width = MIN_WINDOW_SIZE;
                        if (height < MIN_WINDOW_SIZE) height = MIN_WINDOW_SIZE;
                        
                        if (width != event.window.data1 || height != event.window.data2) {
                            SDL_SetWindowSize(window, width, height);
                        }
                        
                        update_window_dimensions(&game, width, height);
                        update_menu_dimensions(&menu, width, height);
                        break;
                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_F11:  // F11 pour basculer le mode plein écran
                        toggle_fullscreen(window, &game);
                        break;
                }
            }
        }

        // Rendu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (menu.mode == MENU_STATE || 
            menu.mode == AI_DIFFICULTY_STATE || 
            menu.mode == GAME_MODE_STATE) {
            render_menu(renderer, &menu);
        } 
        else if (menu.mode == GAME_STATE) {
            render_game(renderer, &game, font);
        }

        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    TTF_CloseFont(font);
    cleanup_menu(&menu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}