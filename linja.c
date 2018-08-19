#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define LEVEL 7

/*
 * Black plays first
 * the board is represented as the position of all the pawns of each color
 * the first array are the black pawns
 */

typedef struct move {
    int color;
    int p1;
    int p2;
} *move;

move init_move(int color, int p1, int p2) {
    move m = malloc(sizeof(struct move));
    assert(m);
    m->color = color;
    m->p1 = p1;
    m->p2 = p2;
    return m;
}

void set_move(move m, int p1, int p2) {
    m->p1 = p1;
    m->p2 = p2;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

void print_board(int board[2][8]) {
    int i, j;
    printf("----------------\n");
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 8; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("----------------\n");
}

void print_move(move m) {
    printf("%d, %d\n", m->p1, m->p2);
}

int is_game_over(int board[2][8]) {
    int black, red = 0;
    for (black = 0; black < 8; black++) {
        if (board[0][black] > 0)
            break;
    }
    for (red = 7; red > -1; red--) {
        if (board[1][red] > 0)
            break;
    }
    if (black > red)
        return 1;
    return 0;
}

int phase1(int board[2][8], int color, int row) {
    board[color][row]--;
    row += color ? -1 : 1;
    board[color][row]++;
    if (row == 0 || row == 7)
        return -1;
    return board[color][row] - 1;
}

void phase2(int board[2][8], int color, int row, int n) {
    board[color][row]--;
    row = color ? max(0, row - n) : min(7, row + n);
    board[color][row]++;
}

void make_move(int board[2][8], move m) {
    int n = phase1(board, m->color, m->p1);
    if (n == -1)
        return;
    phase2(board, m->color, m->p2, n);
}

int is_move_possible(int board[2][8], move m) {
    // check if there is a piece at the target row
    if (board[m->color][m->p1] < 1) {
        return 0;
    }
    // check if there is less than 5 pieces at the arrival row
    int end1_row = m->color ? m->p1 - 1 : m->p1 + 1;
    if (
        (board[0][end1_row] + board[1][end1_row]) > 5
        && end1_row < 7
        && end1_row > 0
    ) {
        return 0;
    }
    // check if there is a piece at the target row
    if (m->p2 == m->p1 && board[m->color][m->p2] < 2) {
        return 0;
    }
    if (end1_row != m->p2 && board[m->color][m->p2] < 1) {
        return 0;
    }
    int n = board[0][end1_row] + board[1][end1_row];
    int end2_row = m->color ? max(0, m->p2 - n) : min(7, m->p2 + n);
    // take previous movement into account
    int max_pieces = end2_row == end1_row ? 4 : 5;
    if (
        (board[0][end2_row] + board[1][end2_row]) > max_pieces
        && end2_row < 7
        && end2_row > 0
    ) {
        return 0;
    }
    return 1;
}

// calculates a score for black (color == 0). If its positive, then he wins
int get_score(int board[2][8]) {
    int i, score = 0;
    for (i = 4; i < 7; i++) {
        score = score + board[0][i] * (i - 3);
    }
    score = score + 5 * board[0][7];
    for (i = 1; i < 4; i++) {
        score = score - board[1][i] * (4 - i);
    }
    score = score - 5 * board[1][0];
    return score;
}

// calculates a score for black (color == 0). If its positive, then he wins
// this is not the score from the rules but for the AI
int get_virtual_score(int board[2][8]) {
    int i, score = 0;
    for (i = 0; i < 4; i++) {
        score = score + board[0][i] * (i - 3);
    }
    for (i = 4; i < 7; i++) {
        score = score + 10 * board[0][i] * (i - 3);
    }
    score = score + 50 * board[0][7];
    for (i = 1; i < 4; i++) {
        score = score - 10 * board[1][i] * (4 - i);
    }
    for (i = 4; i < 8; i++) {
        score = score - board[1][i] * (4 - i);
    }
    score = score - 50 * board[1][0];
    return score;
}

int value_move(int board[2][8], int color, int ttl) {
    if (--ttl < 1) {
        return get_virtual_score(board);
    }
    if (is_game_over(board)) {
        if (get_score(board) > 0)
            return 1000;
        return -1000;
    }
    int new_board[2][8];
    int score = color ? 100 : -100;
    int i, j, s;
    move m = init_move(color, 0, 0);
    move best_move = init_move(color, 0, 0);
    if (color) {
        if (board[1][0] > 0) {
            set_move(best_move, 0, 0);
            memcpy(new_board, board, sizeof(int) * 16);
            make_move(new_board, best_move);
            score = value_move(new_board, 0, ttl);
        }
        for (i = 1; i < 7; i++) {
            for (j = 1; j < 7; j++) {
                set_move(m, i, j);
                if (is_move_possible(board, m)) {
                    memcpy(new_board, board, sizeof(int) * 16);
                    make_move(new_board, m);
                    s = value_move(new_board, 0, ttl);
                    if (s < score) {
                        score = s;
                        set_move(best_move, i, j);
                    }
                }
            }
        }
    } else {
        if (board[0][6] > 0) {
            set_move(best_move, 6, 0);
            memcpy(new_board, board, sizeof(int) * 16);
            make_move(new_board, best_move);
            score = value_move(new_board, 1, ttl);
        }
        for (i = 1; i < 7; i++) {
            for (j = 1; j < 7; j++) {
                set_move(m, i, j);
                if (is_move_possible(board, m)) {
                    memcpy(new_board, board, sizeof(int) * 16);
                    make_move(new_board, m);
                    s = value_move(new_board, 1, ttl);
                    if (s > score) {
                        score = s;
                        set_move(best_move, i, j);
                    }
                }
            }
        }
    }
    free(m);
    free(best_move);
    return score;
}

move best_move_for_black(int board[2][8]) {
    int best_score = -100;
    move best_move = init_move(0, 0, 0);
    int new_board[2][8];
    move m = init_move(0, 0, 0);
    int score = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            set_move(m, i, j);
            if (is_move_possible(board, m)) {
                memcpy(new_board, board, sizeof(int) * 16);
                make_move(new_board, m);
                score = value_move(new_board, 1, LEVEL);
                if (score > best_score) {
                    best_score = score;
                    memcpy(best_move, m, sizeof(struct move));
                }
            }
        }
    }
    free(m);
    printf("best_score: %d\n", best_score);
    return best_move;
}

move interactive_move(int board[2][8]) {
    printf("Input move: ");
    int p1, p2;
    scanf("%d %d", &p1, &p2);
    return init_move(1, p1, p2);
}

int main() {

    int board[2][8] = {
        {2, 0, 0, 1, 2, 1, 5, 0},
        {2, 3, 1, 1, 3, 1, 1, 0},
    };
    print_board(board);
    move m = best_move_for_black(board);
    assert(m);
    make_move(board, m);
    print_move(m);
    print_board(board);
    printf("score: %d\n", get_score(board));
    return 0;

    /*
    int board[2][8] = {
        {6, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 6},
    };

    print_board(board);
    move m;

    while(!is_game_over(board)) {
        m = best_move_for_black(board);
        assert(m);
        make_move(board, m);
        print_move(m);
        print_board(board);
        if (is_game_over(board))
            break;
        m = interactive_move(board);
        assert(m);
        make_move(board, m);
        print_move(m);
        print_board(board);
    }
    if (get_score(board) > 0) {
        printf("Black wins !\n");
    } else {
        printf("Red wins !\n");
    }
    return 0;
    */
}
