/**
 * @file snake.c
 * @brief Implémentation du mini-jeu Snake
 */

#include <stdlib.h>
#include <time.h>
#include "snake.h"

/*********************************
 * Constantes locales
 *********************************/

/** @brief Configuration des animations */
#define ZOOM_SPEED 2.0f

/** @brief Couleurs du jeu */
static const SDL_Color SNAKE_COLOR = {0, 255, 0, 255};  // Vert
static const SDL_Color APPLE_COLOR = {255, 0, 0, 255};  // Rouge
static const SDL_Color BG_COLOR = {0, 0, 0, 255};      // Noir

/*********************************
 * Fonctions utilitaires statiques
 *********************************/

/**
 * @brief Vérifie si la pomme est sur le serpent
 *
 * @param snake État du jeu
 * @param x Position X à vérifier
 * @param y Position Y à vérifier
 * @return 1 si collision, 0 sinon
 */
static int is_apple_on_snake(const snake_t* snake, int x, int y) {
    SnakeSegment* current = snake->head;
    while (current) {
        if (current->x == x && current->y == y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/**
 * @brief Place une nouvelle pomme aléatoirement
 *
 * @param snake État du jeu
 */
static void spawn_new_apple(snake_t* snake) {
    do {
        snake->apple_x = rand() % SNAKE_GRID_SIZE;
        snake->apple_y = rand() % SNAKE_GRID_SIZE;
    } while (is_apple_on_snake(snake, snake->apple_x, snake->apple_y));
}

/**
 * @brief Vérifie les collisions avec les murs
 *
 * @param snake État du jeu
 * @return 1 si collision, 0 sinon
 */
static int check_wall_collision(const snake_t* snake) {
    return (snake->head->x < 0 || snake->head->x >= SNAKE_GRID_SIZE ||
            snake->head->y < 0 || snake->head->y >= SNAKE_GRID_SIZE);
}

/**
 * @brief Vérifie les collisions avec le corps du serpent
 *
 * @param snake État du jeu
 * @return 1 si collision, 0 sinon
 */
static int check_self_collision(const snake_t* snake) {
    SnakeSegment* current = snake->head->next;  // Commence au second segment
    while (current) {
        if (current->x == snake->head->x && current->y == snake->head->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/**
 * @brief Ajoute un segment au serpent
 *
 * @param snake État du jeu
 * @param x Position X du nouveau segment
 * @param y Position Y du nouveau segment
 */
static void add_segment(snake_t* snake, int x, int y) {
    SnakeSegment* new_segment = malloc(sizeof(SnakeSegment));
    if (!new_segment) return;  // Gestion d'erreur d'allocation

    new_segment->x = x;
    new_segment->y = y;
    new_segment->next = NULL;

    // Attache le segment
    if (snake->tail) {
        snake->tail->next = new_segment;
    } else {
        snake->head = new_segment;
    }
    snake->tail = new_segment;
}

/*********************************
 * Implémentation des fonctions publiques
 *********************************/

void init_snake_game(snake_t* snake, int target_row, int target_col, const window_dimensions_t* dimensions) {
    // Position initiale au centre
    int start_x = SNAKE_GRID_SIZE / 2;
    int start_y = SNAKE_GRID_SIZE / 2;

    // Initialisation du serpent
    snake->head = NULL;
    snake->tail = NULL;
    add_segment(snake, start_x, start_y);

    // Configuration initiale
    snake->direction = SNAKE_RIGHT;
    snake->score = 0;
    snake->state = SNAKE_RUNNING;
    snake->zoom_factor = 0.0f;
    snake->target_cell.x = target_col;
    snake->target_cell.y = target_row;

    // Initialise le générateur aléatoire
    srand((unsigned int)time(NULL));
    spawn_new_apple(snake);
    snake->dimensions = *dimensions;  // Copie les dimensions actuelles
}

void update_snake(snake_t* snake) {
    if (snake->state != SNAKE_RUNNING) return;

    // Sauvegarde l'ancienne position de la tête
    int prev_x = snake->head->x;
    int prev_y = snake->head->y;

    // Déplace la tête selon la direction
    switch (snake->direction) {
        case SNAKE_UP:    snake->head->y--; break;
        case SNAKE_DOWN:  snake->head->y++; break;
        case SNAKE_LEFT:  snake->head->x--; break;
        case SNAKE_RIGHT: snake->head->x++; break;
    }

    // Vérifie les collisions
    if (check_wall_collision(snake) || check_self_collision(snake)) {
        snake->state = SNAKE_LOST;
        return;
    }

    // Vérifie si une pomme est mangée
    if (snake->head->x == snake->apple_x && snake->head->y == snake->apple_y) {
        snake->score++;
        
        // Vérifie la victoire
        if (snake->score >= SNAKE_MIN_SCORE) {
            snake->state = SNAKE_WON;
            return;
        }

        // Ajoute un segment et place une nouvelle pomme
        add_segment(snake, prev_x, prev_y);
        spawn_new_apple(snake);
    }

    // Déplace le corps
    SnakeSegment* current = snake->head->next;
    while (current) {
        int temp_x = current->x;
        int temp_y = current->y;
        current->x = prev_x;
        current->y = prev_y;
        prev_x = temp_x;
        prev_y = temp_y;
        current = current->next;
    }
}

void render_snake_game(SDL_Renderer* renderer, const snake_t* snake) {
    const int window_width = snake->dimensions.window_width;
    const int window_height = snake->dimensions.window_height;

    float cell_width = (float)window_width / SNAKE_GRID_SIZE;
    float cell_height = (float)window_height / SNAKE_GRID_SIZE;
    
    float zoom_offset_x = (window_width * snake->zoom_factor) / 2;
    float zoom_offset_y = (window_height * snake->zoom_factor) / 2;

    // Efface l'écran
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);

    float scale = 1.0f - snake->zoom_factor;
    SDL_RenderSetScale(renderer, scale, scale);

    SDL_Rect viewport = {
        (int)zoom_offset_x,
        (int)zoom_offset_y,
        (int)(window_width - 2 * zoom_offset_x),
        (int)(window_height - 2 * zoom_offset_y)
    };
    SDL_RenderSetViewport(renderer, &viewport);

    // Dessine le serpent
    SDL_SetRenderDrawColor(renderer, 
        SNAKE_COLOR.r, SNAKE_COLOR.g, SNAKE_COLOR.b, SNAKE_COLOR.a);
    
    SnakeSegment* current = snake->head;
    while (current) {
        SDL_Rect segment = {
            (int)(current->x * cell_width),
            (int)(current->y * cell_height),
            (int)cell_width - 1,
            (int)cell_height - 1
        };
        SDL_RenderFillRect(renderer, &segment);
        current = current->next;
    }

    // Dessine la pomme
    SDL_SetRenderDrawColor(renderer, 
        APPLE_COLOR.r, APPLE_COLOR.g, APPLE_COLOR.b, APPLE_COLOR.a);
    
    SDL_Rect apple = {
        (int)(snake->apple_x * cell_width),
        (int)(snake->apple_y * cell_height),
        (int)cell_width - 1,
        (int)cell_height - 1
    };
    SDL_RenderFillRect(renderer, &apple);

    // Réinitialise le viewport et l'échelle
        SDL_RenderSetViewport(renderer, NULL);
        SDL_RenderSetScale(renderer, 1.0f, 1.0f);
}

void animate_snake_transition(snake_t* snake, float dt) {
    if (snake->state == SNAKE_WON) {
        snake->zoom_factor += (ZOOM_SPEED * dt);
        if (snake->zoom_factor > 1.0f) {
            snake->zoom_factor = 1.0f;
        }
    } else {
        snake->zoom_factor -= (ZOOM_SPEED * dt);
        if (snake->zoom_factor < 0.0f) {
            snake->zoom_factor = 0.0f;
        }
    }
}

int handle_snake_input(SDL_Event* event, snake_t* snake) {
    if (event->type != SDL_KEYDOWN) {
        return 1;
    }

    SnakeDirection current = snake->direction;
    
    switch (event->key.keysym.sym) {
        case SDLK_UP:
            if (current != SNAKE_DOWN) {
                snake->direction = SNAKE_UP;
            }
            break;
            
        case SDLK_DOWN:
            if (current != SNAKE_UP) {
                snake->direction = SNAKE_DOWN;
            }
            break;
            
        case SDLK_LEFT:
            if (current != SNAKE_RIGHT) {
                snake->direction = SNAKE_LEFT;
            }
            break;
            
        case SDLK_RIGHT:
            if (current != SNAKE_LEFT) {
                snake->direction = SNAKE_RIGHT;
            }
            break;
            
        case SDLK_ESCAPE:
            return 0;  // Quitte le jeu
            
        default:
            break;
    }
    
    return 1;  // Continue le jeu
}

void cleanup_snake(snake_t* snake) {
    SnakeSegment* current = snake->head;
    while (current) {
        SnakeSegment* next = current->next;
        free(current);
        current = next;
    }
    snake->head = NULL;
    snake->tail = NULL;
}

SnakeState play_snake_minigame(game_t* game, SDL_Renderer* renderer, 
                              int row, int col) {
    // Initialisation du mini-jeu
    snake_t snake;
    init_snake_game(&snake, row, col, &game->dimensions);

    // Variables pour la gestion du temps
    Uint32 lastTime = SDL_GetTicks();
    Uint32 accumulator = 0;

    // Boucle principale du mini-jeu
    SDL_Event event;
    int running = 1;
    
    while (running) {
        // Gestion du temps
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += deltaTime;

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || !handle_snake_input(&event, &snake)) {
                snake.state = SNAKE_LOST;
                running = 0;
                break;
            }
        }

        // Mise à jour du jeu à intervalle fixe
        if (accumulator >= SNAKE_SPEED) {
            update_snake(&snake);
            accumulator = 0;
            
            if (snake.state != SNAKE_RUNNING) {
                running = 0;
            }
        }

        // Rendu
        render_snake_game(renderer, &snake);
        SDL_RenderPresent(renderer);
    }

    // Animation de transition
    float transition_time = 0.0f;
    while (transition_time < 1.0f) {
        Uint32 current = SDL_GetTicks();
        float dt = (current - lastTime) / 1000.0f;
        lastTime = current;
        
        animate_snake_transition(&snake, dt);
        transition_time += dt;
        
        render_snake_game(renderer, &snake);
        SDL_RenderPresent(renderer);
    }

    // Sauvegarde le résultat et nettoie
    SnakeState final_state = snake.state;
    cleanup_snake(&snake);
    return final_state;
}