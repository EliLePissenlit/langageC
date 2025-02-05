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

int main(int argc, char *argv[])
{
    // Initialisation de SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not initialize sdl2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Initialisation de SDL_TTF pour le rendu du texte
    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Création de la fenêtre SDL
    SDL_Window *window = SDL_CreateWindow("Tic-Tac-Toe",
                                          100, 100,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Création du renderer avec accélération matérielle et vsync
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Chargement de la police pour le texte
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Initialisation de la structure de jeu
    game_t game = {
        .board = { EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY },
        .player = PLAYER_X,
        .state = RUNNING_STATE,
        .victory_time = 0,
        .is_snake_mode = 0
    };

    // Initialisation des boutons du menu de victoire
    game.replay_button = (SDL_Rect){
        SCREEN_WIDTH / 4,
        2 * SCREEN_HEIGHT / 3,
        SCREEN_WIDTH / 4,
        50
    };

    game.menu_button = (SDL_Rect){
        2 * SCREEN_WIDTH / 4,
        2 * SCREEN_HEIGHT / 3,
        SCREEN_WIDTH / 4,
        50
    };

    // Initialisation des structures de menu et d'IA
    menu_t menu;
    ai_t ai;
    int is_ai_game = 0;
    init_menu(&menu);

    // Boucle principale du jeu
    SDL_Event e;
    while (menu.mode != QUIT_STATE) {
        // Gestion de la transition vers le menu de victoire
        if (game.state == PLAYER_X_WON_STATE || 
            game.state == PLAYER_O_WON_STATE || 
            game.state == TIE_STATE) {
            
            // Si nouvelle victoire, enregistre le temps
            if (game.victory_time == 0) {
                game.victory_time = SDL_GetTicks();
            }
            
            // Après 2 secondes, affiche le menu de victoire
            if (SDL_GetTicks() - game.victory_time >= VICTORY_DISPLAY_TIME) {
                game.state = VICTORY_MENU_STATE;
            }
        }

        // Gestion des événements SDL
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                menu.mode = QUIT_STATE;
                break;

            case SDL_MOUSEBUTTONDOWN:
                // Dans la section case SDL_MOUSEBUTTONDOWN
                if (menu.mode == MENU_STATE || menu.mode == AI_DIFFICULTY_STATE || menu.mode == GAME_MODE_STATE) {
                    int result = handle_menu_click(&menu, e.button.x, e.button.y);
                    if (result == 0) {  // Mode IA sélectionné
                        is_ai_game = 1;
                        game.is_snake_mode = 0;  // Désactive le mode snake pour l'IA
                    } else if (result == 1) {  // Mode JcJ sélectionné
                        is_ai_game = 0;
                    } else if (result == MODE_CLASSIC) {  // Mode classique sélectionné
                        is_ai_game = 0;
                        game.is_snake_mode = 0;
                        menu.mode = GAME_STATE;
                    } else if (result == MODE_SNAKE) {  // Mode snake sélectionné
                        is_ai_game = 0;
                        game.is_snake_mode = 1;
                        menu.mode = GAME_STATE;
                    } else if (result >= EASY && result <= HARD) {  // Difficulté IA sélectionnée
                        init_ai(&ai, result);
                        is_ai_game = 1;
                        game.is_snake_mode = 0;  // Désactive explicitement le mode snake pour l'IA
                        menu.mode = GAME_STATE;
                    }
                }
                else if (menu.mode == GAME_STATE) {
                    if (game.state == VICTORY_MENU_STATE) {
                        // Gestion des boutons du menu de victoire
                        if (SDL_PointInRect(&(SDL_Point){e.button.x, e.button.y}, 
                                          &game.replay_button)) {
                            reset_game(&game);
                            game.victory_time = 0;
                        }
                        else if (SDL_PointInRect(&(SDL_Point){e.button.x, e.button.y}, 
                                               &game.menu_button)) {
                            reset_game(&game);
                            game.victory_time = 0;
                            menu.mode = MENU_STATE;
                            is_ai_game = 0;
                        }
                    }
                    // Dans la section du traitement des clics pendant le jeu
                    else if (game.state == RUNNING_STATE) {
                        int row = e.button.y / CELL_HEIGHT;
                        int col = e.button.x / CELL_WIDTH;
                        
                        // Vérifie si la case est vide avant de lancer le snake
                        if (game.board[row * N + col] == EMPTY) {
                            if (game.is_snake_mode) {  // Supprimé la condition game.player == PLAYER_X
                                // Lance le mini-jeu snake
                                SnakeState result = play_snake_minigame(&game, renderer, row, col);
                                
                                if (result == SNAKE_WON) {
                                    // Le joueur a gagné au snake, il peut placer son symbole
                                    click_on_cell(&game, row, col);
                                    
                                    // Si mode IA, fait jouer l'IA après le joueur
                                    if (is_ai_game && 
                                        game.state == RUNNING_STATE && 
                                        game.player == PLAYER_O) {
                                        ai_make_move(&game, &ai);
                                    }
                                }
                                // Si SNAKE_LOST, le joueur perd son tour
                                if (result == SNAKE_LOST) {
                                    switch_player(&game);  // Utilise la fonction existante pour changer de joueur
                                    if (is_ai_game && game.player == PLAYER_O) {
                                        ai_make_move(&game, &ai);
                                    }
                                }
                            } else {
                                // Mode classique
                                click_on_cell(&game, row, col);
                                
                                // Si mode IA, fait jouer l'IA après le joueur
                                if (is_ai_game && 
                                    game.state == RUNNING_STATE && 
                                    game.player == PLAYER_O) {
                                    ai_make_move(&game, &ai);
                                }
                            }
                        }
                    }
                }
                break;

            default: {}
            }
        }

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Rendu de l'interface appropriée
        if (menu.mode == MENU_STATE || 
            menu.mode == AI_DIFFICULTY_STATE || 
            menu.mode == GAME_MODE_STATE) {
            // Affiche le menu approprié selon l'état
            render_menu(renderer, &menu);
        } 
        else if (menu.mode == GAME_STATE) {
            render_game(renderer, &game, font);
        }

        // Mise à jour de l'affichage
        SDL_RenderPresent(renderer);
    }

    // Nettoyage des ressources
    TTF_CloseFont(font);
    cleanup_menu(&menu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}