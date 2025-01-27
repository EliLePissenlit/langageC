#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    bool against_ai;
    int level;
} GameConfig;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define CELL_SIZE (WINDOW_WIDTH / 3)

char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
bool player1_turn = true;

void draw_board(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH/3, 0, WINDOW_WIDTH/3, WINDOW_HEIGHT);
    SDL_RenderDrawLine(renderer, 2*WINDOW_WIDTH/3, 0, 2*WINDOW_WIDTH/3, WINDOW_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT/3, WINDOW_WIDTH, WINDOW_HEIGHT/3);
    SDL_RenderDrawLine(renderer, 0, 2*WINDOW_HEIGHT/3, WINDOW_WIDTH, 2*WINDOW_HEIGHT/3);
}

void draw_x(SDL_Renderer* renderer, int row, int col) {
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;
    int padding = CELL_SIZE / 4;
    SDL_RenderDrawLine(renderer, x + padding, y + padding,
                       x + CELL_SIZE - padding, y + CELL_SIZE - padding);
    SDL_RenderDrawLine(renderer, x + CELL_SIZE - padding, y + padding,
                       x + padding, y + CELL_SIZE - padding);
}

void draw_o(SDL_Renderer* renderer, int row, int col) {
    int x = col * CELL_SIZE + CELL_SIZE/2;
    int y = row * CELL_SIZE + CELL_SIZE/2;
    int radius = CELL_SIZE/3;
    for(int w = -radius; w <= radius; w++) {
        for(int h = -radius; h <= radius; h++) {
            if(w*w + h*h <= radius*radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

void draw_pieces(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == 'X') draw_x(renderer, i, j);
            else if(board[i][j] == 'O') draw_o(renderer, i, j);
        }
    }
}

bool check_winner() {
    for(int i = 0; i < 3; i++) {
        if(board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return true;
        if(board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return true;
    }
    if(board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return true;
    if(board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return true;
    return false;
}

bool is_board_full() {
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(board[i][j] == ' ') return false;
    return true;
}

typedef struct {
    int row;
    int col;
} Move;

Move get_ai_move_easy() {
    Move move;
    do {
        move.row = rand() % 3;
        move.col = rand() % 3;
    } while(board[move.row][move.col] != ' ');
    return move;
}

Move get_ai_move_medium() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = 'O';
                if(check_winner()) {
                    board[i][j] = ' ';
                    return (Move){i, j};
                }
                board[i][j] = 'X';
                if(check_winner()) {
                    board[i][j] = ' ';
                    return (Move){i, j};
                }
                board[i][j] = ' ';
            }
        }
    }
    return get_ai_move_easy();
}

int minimax(bool is_maximizing) {
    if(check_winner()) return is_maximizing ? -1 : 1;
    if(is_board_full()) return 0;
    
    int best_score = is_maximizing ? -1000 : 1000;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = is_maximizing ? 'O' : 'X';
                int score = minimax(!is_maximizing);
                board[i][j] = ' ';
                best_score = is_maximizing ? 
                    (score > best_score ? score : best_score) :
                    (score < best_score ? score : best_score);
            }
        }
    }
    return best_score;
}

Move get_ai_move_hard() {
    Move best_move = {-1, -1};
    int best_score = -1000;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = 'O';
                int score = minimax(false);
                board[i][j] = ' ';
                if(score > best_score) {
                    best_score = score;
                    best_move.row = i;
                    best_move.col = j;
                }
            }
        }
    }
    return best_move;
}

Move get_ai_move(int level) {
    switch(level) {
        case 1: return get_ai_move_easy();
        case 2: return get_ai_move_medium();
        case 3: return get_ai_move_hard();
        default: return get_ai_move_easy();
    }
}

GameConfig show_level_selection() {
    SDL_Window* window = SDL_CreateWindow("Choisir le niveau",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        300, 400,
                                        SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    
    GameConfig config = {true, 1};
    bool selected = false;
    SDL_Event event;
    SDL_Rect buttons[3] = {
        {50, 100, 200, 50},
        {50, 200, 200, 50},
        {50, 300, 200, 50}
    };
    
    while(!selected) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(EXIT_SUCCESS);
            }
            if(event.type == SDL_MOUSEBUTTONDOWN) {
                for(int i = 0; i < 3; i++) {
                    if(mouse_x >= buttons[i].x && mouse_x <= buttons[i].x + buttons[i].w &&
                       mouse_y >= buttons[i].y && mouse_y <= buttons[i].y + buttons[i].h) {
                        config.level = i + 1;
                        selected = true;
                        break;
                    }
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        for(int i = 0; i < 3; i++) {
            bool hover = mouse_x >= buttons[i].x && mouse_x <= buttons[i].x + buttons[i].w &&
                        mouse_y >= buttons[i].y && mouse_y <= buttons[i].y + buttons[i].h;
            
            SDL_SetRenderDrawColor(renderer, hover ? 70 : 50, hover ? 140 : 120, hover ? 210 : 190, 255);
            SDL_RenderFillRect(renderer, &buttons[i]);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &buttons[i]);
            
            const char* text = i == 0 ? "NIVEAU 1" : (i == 1 ? "NIVEAU 2" : "NIVEAU 3");
            int len = i == 0 ? 8 : (i == 1 ? 8 : 8);
            for(int j = 0; j < len; j++) {
                int x = buttons[i].x + buttons[i].w/2 - (len * 10) + j*20;
                int y = buttons[i].y + buttons[i].h/2;
                for(int dx = -2; dx <= 2; dx++) {
                    for(int dy = -2; dy <= 2; dy++) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                    }
                }
            }
        }
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return config;
}

int main() {
    srand(time(NULL));
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }
    
    GameConfig config = show_level_selection();
    
    SDL_Window* window = SDL_CreateWindow("Tic Tac Toe",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if(!window) {
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    
    bool quit = false;
    SDL_Event event;
    
    while(!quit) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = true;
            } else if(event.type == SDL_MOUSEBUTTONDOWN && !check_winner() && player1_turn) {
                int x = event.button.x / CELL_SIZE;
                int y = event.button.y / CELL_SIZE;
                
                if(x >= 0 && x < 3 && y >= 0 && y < 3 && board[y][x] == ' ') {
                    board[y][x] = 'X';
                    player1_turn = false;
                }
            }
        }
        
        if(!player1_turn && !check_winner() && !is_board_full()) {
            Move ai_move = get_ai_move(config.level);
            board[ai_move.row][ai_move.col] = 'O';
            player1_turn = true;
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        draw_board(renderer);
        draw_pieces(renderer);
        
        SDL_RenderPresent(renderer);
        
        if(check_winner() || is_board_full()) {
            SDL_Delay(2000);  
            quit = true;
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    exit(EXIT_SUCCESS);
}