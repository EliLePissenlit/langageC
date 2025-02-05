// rendering.h
#ifndef RENDERING_H_
#define RENDERING_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./game.h"

// Fonction principale de rendu du jeu
void render_game(SDL_Renderer *renderer, const game_t *game, TTF_Font *font);

// Fonction de rendu du menu de victoire
void render_victory_menu(SDL_Renderer *renderer, const game_t *game, TTF_Font *font);

#endif  // RENDERING_H_

