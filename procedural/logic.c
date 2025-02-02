#include "logic.h"
#include <stdio.h>

void click_on_cell(int row, int col, int player, int board[3][3]) {
    if (board[row][col] == 0) {
        board[row][col] = player;
    }
}

int check_victory(int board[3][3]) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
        if (board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
    }
    if (board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[0][0];
    if (board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[0][2];
    return 0;
}
