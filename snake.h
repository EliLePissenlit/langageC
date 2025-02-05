#ifndef SNAKE_H_
#define SNAKE_H_

#include <SDL2/SDL.h>
#include "./game.h"

// États du mode Snake
typedef enum {
    SNAKE_NONE,
    SNAKE_RUNNING,
    SNAKE_WON,
    SNAKE_LOST
} SnakeState;

// Structure du serpent
typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment* next;
} SnakeSegment;

// Structure principale du Snake
typedef struct {
    SnakeSegment* head;
    SnakeSegment* tail;
    int direction;
    int apple_x;
    int apple_y;
    int score;
    SnakeState state;
    float zoom_factor; // Pour l'animation de transition
    SDL_Point target_cell; // Case du morpion ciblée
} snake_t;

// Constantes pour le Snake
#define SNAKE_GRID_SIZE 20
#define SNAKE_MIN_SCORE 5 // Score minimum pour gagner
#define SNAKE_SPEED 100 // Délai en ms

// Directions
enum {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT
};

// Fonctions principales
void init_snake_game(snake_t* snake, int target_row, int target_col);
void update_snake(snake_t* snake);
void render_snake_game(SDL_Renderer* renderer, const snake_t* snake);
int handle_snake_input(SDL_Event* event, snake_t* snake);
void cleanup_snake(snake_t* snake);
void animate_snake_transition(snake_t* snake, float dt);

// Fonction d'intégration
SnakeState play_snake_minigame(game_t* game, SDL_Renderer* renderer, int row, int col);

#endif // SNAKE_H_