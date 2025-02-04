#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // Inclure SDL_ttf
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// SCREEN CONFIG
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GRID_SIZE 30
#define CELL_SIZE (WINDOW_WIDTH / GRID_SIZE)

typedef struct {
    int x, y;
} Apple;

typedef struct Snake {
    int x, y, dir;
    struct Snake *next;
} Snake;

Snake *head, *tail;
Apple apple1, apple2;
int score = 0;

// Directions
enum { SNAKE_UP, SNAKE_DOWN, SNAKE_LEFT, SNAKE_RIGHT };

// Déclaration de la police TTF
TTF_Font *font;

void init_snake() {
    Snake *new = malloc(sizeof(Snake));
    new->x = GRID_SIZE / 2;
    new->y = GRID_SIZE / 2;
    new->dir = SNAKE_RIGHT;
    new->next = NULL;                    //tout ca créée un nouveau snake au milieu de la grille allant vers la droite
    
    head = tail = new;            // la tete et la queue pointent vers la meme direction
}

void increase_snake() {
    Snake *new = malloc(sizeof(Snake));
    new->x = tail->x;
    new->y = tail->y;
    new->dir = tail->dir;

    switch (tail->dir) {
        case SNAKE_UP: new->y += 1; break;
        case SNAKE_DOWN: new->y -= 1; break;
        case SNAKE_LEFT: new->x += 1; break;
        case SNAKE_RIGHT: new->x -= 1; break;                          // ici on vérifie vers où va le serpent, et on rajoute un élément a l'arriere de sa queue pour l'aggrandir quand il mange un fruit
    }

    new->next = NULL;
    tail->next = new;           //on relie l’ancienne queue au nouveau segment
    tail = new;                 //la nouvelle queue devient new car c'est le nouveau segment
}

void gen_apple(Apple *apple) {
    int min_x = 2;  // Marge minimale pour éviter les bords
    int min_y = 2;
    int max_x = 18; // Taille de la zone jouable basée sur la fenêtre
    int max_y = 18;

    apple->x = min_x + rand() % (max_x - min_x);    //génère un fruit aléatoirement dans la grille, La pomme reçoit des coordonnées aléatoires comprises entre min_x et max_x, et min_y et max_y
    apple->y = min_y + rand() % (max_y - min_y);
}

void detect_apple(int *running, int *game_result) {
    if (head->x == apple1.x && head->y == apple1.y) {               // on vérifie si le serpent est sur la pomme, et si oui on augmente le score + le serpent + on génère une nouvelle pomme
        score++;
        increase_snake();
        gen_apple(&apple1);
    }

    if (head->x == apple2.x && head->y == apple2.y) {               // pareil pour la pomme 2
        score++;
        increase_snake();
        gen_apple(&apple2);
    }

    if (score >= 15) {                                 // si le score arrive a 15, on arrête le jeu avec un délai de 500ms et on affiche la victoire
        SDL_Delay(500);
        *running = 0;
        *game_result = 1;            // 1 = Victoire
    }
}

void move_snake() {
    int prev_x = head->x, prev_y = head->y, prev_dir = head->dir;           // on mémorise la position et la direction de la tete du serpent

    switch (head->dir) {                              // selon la direction de la tete, on modifie ses coordonnées
        case SNAKE_UP: head->y--; break;
        case SNAKE_DOWN: head->y++; break;
        case SNAKE_LEFT: head->x--; break;
        case SNAKE_RIGHT: head->x++; break;
    }

    Snake *track = head->next;                //ici on parcourt chaque segment pour le placer a l'emplacement du segment précédent, et on s'arrete quand il n'y a plus de "track"
    while (track) {
        int new_x = track->x, new_y = track->y, new_dir = track->dir;
        track->x = prev_x;
        track->y = prev_y;
        track->dir = prev_dir;
        prev_x = new_x;
        prev_y = new_y;
        prev_dir = new_dir;
        track = track->next;              //on passe au segment suivant
    }
}

void detect_crash(int *running, SDL_Window *window, int *game_result) {
    int real_w, real_h;
    SDL_GetWindowSize(window, &real_w, &real_h);           //donne les dimensions de la fenêtre en pixels
    
    int max_x = real_w / CELL_SIZE;     // on divise la largeur et la hauteur par la taille d'une cellule pour avoir le nombre de cellules
    int max_y = (real_h / CELL_SIZE) - 1;  // -1 pour la marge de sécurité

    if (head->x < 0 || head->x >= max_x || head->y < 0 || head->y >= max_y) {         // on vérifie si la tete du serpent est en dehors de la grille, si oui on arrête le jeu avec un délai de 500ms, défaite  
        SDL_Delay(500);
        *running = 0;
        *game_result = -1;  // -1 = Défaite
        return;
    }

    Snake *track = head->next;                    // ici on vérifie si la tete du serpent est sur un segment de son corps, si oui on arrête le jeu avec un délai de 500ms, défaite
    while (track) {
        if (head->x == track->x && head->y == track->y) {
            SDL_Delay(500);
            *running = 0;
            *game_result = -1;  // -1 = Défaite
            return;
        }
        track = track->next;
    }
}

void render_snake(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);                         // on donne la couleur au serpen ici, 0 = rouge, 255 = vert, 0 = bleu, 255 = opacité
    Snake *track = head;
    SDL_Rect rect = {0, 0, CELL_SIZE, CELL_SIZE};

    while (track) {                              // on parcourt chaque segment du serpent et on le dessine
        rect.x = track->x * CELL_SIZE;
        rect.y = track->y * CELL_SIZE;
        SDL_RenderFillRect(renderer, &rect);
        track = track->next;
    }
}

void render_apple(SDL_Renderer *renderer, Apple apple) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);                          // pareil qu'avant, on donne la couleur rouge a la pomme
    SDL_Rect rect = {apple.x * CELL_SIZE, apple.y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void render_score(SDL_Renderer *renderer) {
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Score: %d", score);

    SDL_Color color = {255, 255, 255, 255};                   //couleur blanche
    SDL_Surface *surface = TTF_RenderText_Solid(font, score_text, color);          //fonction TTF_render_text_solid pour rendre du texte sur une surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);        // crée une texture a partir de la surface rendue juste avant

    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {10, 10, texW, texH};              // on place le texte en haut a gauche

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);         // la on copie la texture sur le renderer

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);            // on libère la mémoire de la surface et de la texture
}

int main() {
    srand(time(0));                                   //Initialise la graine de nombres aléatoires. Cela permet à rand() de donner des valeurs différentes à chaque exécution du programme
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    int running = 1;
    int game_result = 0;  // 1 = Victoire, -1 = Défaite

    font = TTF_OpenFont("DejaVuSans.ttf", 24);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);         // crée une fenêtre de dimensions width x height en pixels et la centre sur l'écran
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);           // associe un renderer à la fenêtre, -1 pour le driver, et SDL_RENDERER_ACCELERATED pour le mode accéléré

    init_snake();
    gen_apple(&apple1);
    gen_apple(&apple2);

    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {            // on vérifie les événements
            if (event.type == SDL_QUIT) running = 0;         // si on clique sur la croix de la fenetre, on arrête le jeu
            else if (event.type == SDL_KEYDOWN) {         // si on appuie sur une touche, on vérifie laquelle et on modifie la direction du serpent
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_UP: if (head->dir != SNAKE_DOWN) head->dir = SNAKE_UP; break;
                    case SDLK_DOWN: if (head->dir != SNAKE_UP) head->dir = SNAKE_DOWN; break;
                    case SDLK_LEFT: if (head->dir != SNAKE_RIGHT) head->dir = SNAKE_LEFT; break;
                    case SDLK_RIGHT: if (head->dir != SNAKE_LEFT) head->dir = SNAKE_RIGHT; break;
                }
            }
        }

        detect_apple(&running, &game_result);
        move_snake();
        detect_crash(&running, window, &game_result);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);        // couleur en noir
        SDL_RenderClear(renderer);            // on efface le renderer
        render_snake(renderer);        // on dessine le serpent et les pommes
        render_apple(renderer, apple1); 
        render_apple(renderer, apple2);
        render_score(renderer);       // on affiche le score
        SDL_RenderPresent(renderer);      // on actualise l'écran

        SDL_Delay(80);      // petit délai de 80ms pour réguler la vitesse du serpent
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return game_result;  // Retourne 1 (victoire) ou -1 (défaite)

}
