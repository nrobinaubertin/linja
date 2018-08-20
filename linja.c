#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define LEVEL 8

/*
 * Black plays first
 * the board is represented as the position of all the pawns of each color
 * the first array are the black pawns
 */

typedef struct move {
    int color; // 0 = black, 1 = red
    int p1; // phase1 row
    int end_p1; // end row of phase1 move
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

int phase1(int board[2][8], int color, int row, int end_row) {
    board[color][row]--;
    board[color][end_row]++;
    if (end_row == 0 || end_row == 7)
        return -1;
    return board[color][end_row] - 1;
}

void phase2(int board[2][8], int color, int row, int n) {
    board[color][row]--;
    row = color ? max(0, row - n) : min(7, row + n);
    board[color][row]++;
}

void make_move(int board[2][8], move m) {
    int n = phase1(board, m->color, m->p1, m->end_p1);
    if (n == -1)
        return;
    phase2(board, m->color, m->p2, n);
}

int is_move_possible(int board[2][8], move m) {
    // check if there is a piece at the target row
    if (m->p2 == m->p1 && board[m->color][m->p2] < 2) {
        return 0;
    }
    if (m->end_p1 != m->p2 && board[m->color][m->p2] < 1) {
        return 0;
    }
    int n = board[0][m->end_p1] + board[1][m->end_p1];
    int end2_row = m->color ? max(0, m->p2 - n) : min(7, m->p2 + n);
    // take previous movement into account
    int max_pieces = end2_row == m->end_p1 ? 4 : 5;
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
        // color == 1 // RED
        if (board[1][0] > 0) {
            best_move->p1 = 1;
            best_move->end_p1 = 0;
            best_move->p2 = 0;
            memcpy(new_board, board, sizeof(int) * 16);
            make_move(new_board, best_move);
            score = value_move(new_board, 0, ttl);
        }
        for (i = 2; i < 8; i++) {
            // check if there is a piece at the target row
            if (board[1][i] < 1) {
                continue;
            }

            m->p1 = i;
            m->end_p1 = i - 1;
            // check if there is less than 5 pieces at the arrival row
            if (board[0][m->end_p1] + board[1][m->end_p1] > 5) {
                continue;
            }
            for (j = 1; j < 7; j++) {
                m->p2 = j;
                if (is_move_possible(board, m)) {
                    memcpy(new_board, board, sizeof(int) * 16);
                    make_move(new_board, m);
                    s = value_move(new_board, 0, ttl);
                    if (s < score) {
                        score = s;
                        memcpy(best_move, m, sizeof(struct move));
                    }
                }
            }
        }
    } else {
        // color == 0 // BLACK
        if (board[0][6] > 0) {
            best_move->p1 = 6;
            best_move->end_p1 = 7;
            best_move->p2 = 0;
            memcpy(new_board, board, sizeof(int) * 16);
            make_move(new_board, best_move);
            score = value_move(new_board, 1, ttl);
        }
        for (i = 0; i < 6; i++) {
            // check if there is a piece at the target row
            if (board[0][i] < 1) {
                continue;
            }

            m->p1 = i;
            m->end_p1 = i + 1;
            // check if there is less than 5 pieces at the arrival row
            if (board[0][m->end_p1] + board[1][m->end_p1] > 5) {
                continue;
            }
            for (j = 1; j < 7; j++) {
                m->p2 = j;
                if (is_move_possible(board, m)) {
                    memcpy(new_board, board, sizeof(int) * 16);
                    make_move(new_board, m);
                    s = value_move(new_board, 1, ttl);
                    if (s > score) {
                        score = s;
                        memcpy(best_move, m, sizeof(struct move));
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
        // check if there is a piece at the target row
        if (board[0][i] < 1) {
            continue;
        }
        // check if there is less than 5 pieces at the arrival row
        if (i < 6 && (board[0][i + 1] + board[1][i + 1]) > 5) {
            continue;
        }
        for (int j = 0; j < 7; j++) {
            m->p1 = i;
            m->end_p1 = i + 1;
            m->p2 = j;
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
