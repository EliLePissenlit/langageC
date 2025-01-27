#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
            if(board[i][j] == 'X') {
                draw_x(renderer, i, j);
            } else if(board[i][j] == 'O') {
                draw_o(renderer, i, j);
            }
        }
    }
}

bool check_winner() {
    for(int i = 0; i < 3; i++) {
        if(board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return true;
        }
    }
    
    for(int i = 0; i < 3; i++) {
        if(board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return true;
        }
    }
    
    if(board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return true;
    }
    if(board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return true;
    }
    
    return false;
}

bool is_board_full() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

int main() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }
    
    SDL_Window* window = SDL_CreateWindow("Tic Tac Toe",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if(!window) {
        SDL_Quit();
         exit(EXIT_FAILURE);
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                              SDL_RENDERER_ACCELERATED);
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
            } else if(event.type == SDL_MOUSEBUTTONDOWN && !check_winner()) {
                int x = event.button.x / CELL_SIZE;
                int y = event.button.y / CELL_SIZE;
                
                if(x >= 0 && x < 3 && y >= 0 && y < 3 && board[y][x] == ' ') {
                    board[y][x] = player1_turn ? 'X' : 'O';
                    player1_turn = !player1_turn;
                }
            }
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