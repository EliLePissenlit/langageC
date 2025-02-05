/**
 * @file window.h
 * @brief Gestion de la fenêtre et du redimensionnement
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL2/SDL.h>
#include "./game.h"

// Dimensions initiales de la fenêtre
#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480

// Dimension minimale de la fenêtre
#define MIN_WINDOW_SIZE 300

void update_window_dimensions(game_t* game, int width, int height);
void toggle_fullscreen(SDL_Window* window, game_t* game);

#endif /* WINDOW_H_ */