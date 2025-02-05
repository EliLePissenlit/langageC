#include "snake.h"
#include <stdlib.h>
#include <time.h>

// Fonction auxiliaire pour vérifier si la pomme est sur le serpent
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

void init_snake_game(snake_t* snake, int target_row, int target_col) {
    // Initialise le serpent au centre
    SnakeSegment* initial = malloc(sizeof(SnakeSegment));
    initial->x = SNAKE_GRID_SIZE / 2;
    initial->y = SNAKE_GRID_SIZE / 2;
    initial->next = NULL;
    
    snake->head = snake->tail = initial;
    snake->direction = SNAKE_RIGHT;
    snake->score = 0;
    snake->state = SNAKE_RUNNING;
    snake->zoom_factor = 0.0f;
    snake->target_cell.x = target_col;
    snake->target_cell.y = target_row;
    
    // Place la pomme aléatoirement
    srand(time(NULL));
    do {
        snake->apple_x = rand() % SNAKE_GRID_SIZE;
        snake->apple_y = rand() % SNAKE_GRID_SIZE;
    } while (is_apple_on_snake(snake, snake->apple_x, snake->apple_y));
}

void animate_snake_transition(snake_t* snake, float dt) {
    // Animation de zoom/dézoom
    const float ZOOM_SPEED = 2.0f;
    if (snake->state == SNAKE_WON) {
        snake->zoom_factor = snake->zoom_factor + (ZOOM_SPEED * dt);
        if (snake->zoom_factor > 1.0f) snake->zoom_factor = 1.0f;
    } else {
        snake->zoom_factor = snake->zoom_factor - (ZOOM_SPEED * dt);
        if (snake->zoom_factor < 0.0f) snake->zoom_factor = 0.0f;
    }
}

void update_snake(snake_t* snake) {
    if (snake->state != SNAKE_RUNNING) return;

    // Sauvegarde l'ancienne position
    int prev_x = snake->head->x;
    int prev_y = snake->head->y;

    // Déplace la tête
    switch (snake->direction) {
        case SNAKE_UP: snake->head->y--; break;
        case SNAKE_DOWN: snake->head->y++; break;
        case SNAKE_LEFT: snake->head->x--; break;
        case SNAKE_RIGHT: snake->head->x++; break;
    }

    // Vérification des collisions avec les murs
    if (snake->head->x < 0 || snake->head->x >= SNAKE_GRID_SIZE ||
        snake->head->y < 0 || snake->head->y >= SNAKE_GRID_SIZE) {
        snake->state = SNAKE_LOST;
        return;
    }

    // Vérification des collisions avec la pomme
    if (snake->head->x == snake->apple_x && snake->head->y == snake->apple_y) {
        snake->score++;
        if (snake->score >= SNAKE_MIN_SCORE) {
            snake->state = SNAKE_WON;
            return;
        }

        // Ajoute un nouveau segment
        SnakeSegment* new_segment = malloc(sizeof(SnakeSegment));
        new_segment->x = prev_x;
        new_segment->y = prev_y;
        new_segment->next = NULL;
        snake->tail->next = new_segment;
        snake->tail = new_segment;

        // Nouvelle position pour la pomme
        do {
            snake->apple_x = rand() % SNAKE_GRID_SIZE;
            snake->apple_y = rand() % SNAKE_GRID_SIZE;
        } while (is_apple_on_snake(snake, snake->apple_x, snake->apple_y));
    }

    // Déplace le reste du corps
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
    // Calcule la taille des cellules en fonction du zoom
    float cell_width = SCREEN_WIDTH / SNAKE_GRID_SIZE;
    float cell_height = SCREEN_HEIGHT / SNAKE_GRID_SIZE;
    
    // Calcul des décalages pour le centrage avec le zoom
    float zoom_offset_x = (SCREEN_WIDTH * snake->zoom_factor) / 2;
    float zoom_offset_y = (SCREEN_HEIGHT * snake->zoom_factor) / 2;

    // Effacement de l'écran
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Application du zoom et du centrage
    float scale = 1.0f - snake->zoom_factor;
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_RenderSetViewport(renderer, &(SDL_Rect){
        zoom_offset_x,
        zoom_offset_y,
        SCREEN_WIDTH - 2 * zoom_offset_x,
        SCREEN_HEIGHT - 2 * zoom_offset_y
    });

    // Dessine le serpent
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SnakeSegment* current = snake->head;
    while (current) {
        SDL_Rect segment = {
            current->x * cell_width,
            current->y * cell_height,
            cell_width - 1,
            cell_height - 1
        };
        SDL_RenderFillRect(renderer, &segment);
        current = current->next;
    }

    // Dessine la pomme
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect apple = {
        snake->apple_x * cell_width,
        snake->apple_y * cell_height,
        cell_width - 1,
        cell_height - 1
    };
    SDL_RenderFillRect(renderer, &apple);

    // Réinitialise le viewport et l'échelle
    SDL_RenderSetViewport(renderer, NULL);
    SDL_RenderSetScale(renderer, 1.0f, 1.0f);
}

int handle_snake_input(SDL_Event* event, snake_t* snake) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                if (snake->direction != SNAKE_DOWN)
                    snake->direction = SNAKE_UP;
                break;
            case SDLK_DOWN:
                if (snake->direction != SNAKE_UP)
                    snake->direction = SNAKE_DOWN;
                break;
            case SDLK_LEFT:
                if (snake->direction != SNAKE_RIGHT)
                    snake->direction = SNAKE_LEFT;
                break;
            case SDLK_RIGHT:
                if (snake->direction != SNAKE_LEFT)
                    snake->direction = SNAKE_RIGHT;
                break;
            case SDLK_ESCAPE:
                return 0;
        }
    }
    return 1;
}

void cleanup_snake(snake_t* snake) {
    SnakeSegment* current = snake->head;
    while (current) {
        SnakeSegment* next = current->next;
        free(current);
        current = next;
    }
    snake->head = snake->tail = NULL;
}

SnakeState play_snake_minigame(game_t* game, SDL_Renderer* renderer, int row, int col) {
    snake_t snake;
    init_snake_game(&snake, row, col);

    Uint32 lastTime = SDL_GetTicks();
    Uint32 accumulator = 0;

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
            }
        }

        // Mise à jour du jeu
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

    SnakeState final_state = snake.state;
    cleanup_snake(&snake);
    return final_state;
}