#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define LEVEL 7

/*
 * black plays first
 * the board is represented as the position of all the pawns of each color
 * the first row of a board are the black pawns
 * a board is an int[2][8] or int**
 * a board_list is a board[64] or int***
 */

int** create_board() {
    int** board = malloc(sizeof(int*) * 2);
    for (int i = 0; i < 2; i++) {
        board[i] = (int*) malloc(sizeof(int) * 8);
    }
    assert(board[0] && board[1]);
    return board;
}

void destroy_board(int** board) {
    if (board) {
        for (int i = 0; i < 2; i++) {
            if (board[i])
                free(board[i]);
        }
        free(board);
    }
}

int** copy_board(int** new_board, int** board) {
    memcpy(new_board[0], board[0], sizeof(int) * 8);
    memcpy(new_board[1], board[1], sizeof(int) * 8);
    return new_board;
}

int boardcmp(int** b1, int** b2) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            if (b1[i][j] != b2[i][j])
                return 0;
        }
    }
    return 1;
}

void print_board(int** board) {
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

void print_board_list(int*** board_list) {
    printf("\n\n______\n\n");
    for(int i = 0; i < 64; i++) {
        if (board_list[i] == NULL) {
            printf("NULL,");
        } else {
            print_board(board_list[i]);
        }
    }
    printf("\n\n______\n\n");
}

int is_in_board_list(int*** board_list, int** b) {
    for(int i = 0; i < 64; i++) {
        if (board_list[i] == NULL)
            return 0;
        if (boardcmp(board_list[i], b))
            return 1;
    }
    return 0;
}

void add_to_board_list(int*** board_list, int** b) {
    int i = 0;
    while(board_list[i] != NULL) {
        i++;
    }
    board_list[i] = b;
}

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

void print_move(move m) {
    printf("%d, %d\n", m->p1, m->p2);
}

int is_game_over(int** board) {
    assert(board[0] && board[1]);
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

int phase1(int** board, int color, int row, int end_row) {
    assert(row > -1 && row < 8);
    assert(end_row > -1 && end_row < 8);
    assert(color == 0 || color == 1);
    assert(board[0] && board[1]);
    board[color][row]--;
    board[color][end_row]++;
    if (end_row == 0 || end_row == 7)
        return -1;
    return board[color][end_row] - 1;
}

void phase2(int** board, int color, int row, int n) {
    assert(row > -1 && row < 8);
    assert(n > -1);
    assert(color == 0 || color == 1);
    assert(board[0] && board[1]);
    board[color][row]--;
    row = color ? max(0, row - n) : min(7, row + n);
    assert(row > -1 && row < 8);
    board[color][row]++;
}

void make_move(int** board, move m) {
    int n = phase1(board, m->color, m->p1, m->end_p1);
    if (n == -1)
        return;
    phase2(board, m->color, m->p2, n);
}

// returns the new_board if move okay, NULL otherwise
int** is_move_possible(int** board, move m) {
    assert(m->p1 > -1 && m->p1 < 8);
    assert(board[0] && board[1]);
    assert(m);
    int** new_board = create_board();
    copy_board(new_board, board);
    make_move(new_board, m);

    for(int i = 0; i < 2; i++) {
        for(int j = 1; j < 7; j++) {
            if (new_board[i][j] < 0 || new_board[i][j] > 5) {
                destroy_board(new_board);
                return NULL;
            }
        }
    }
    return new_board;
}

// calculates a score for black (color == 0). If its positive, then he wins
int get_score(int** board) {
    assert(board[0] && board[1]);
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
int get_virtual_score(int** board) {
    assert(board[0] && board[1]);
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

int value_move(int** board, int color, int ttl) {
    assert(board[0] && board[1]);
    if (--ttl < 1) {
        return get_virtual_score(board);
    }
    if (is_game_over(board)) {
        if (get_score(board) > 0)
            return 1000;
        return -1000;
    }
    int i, j, s;
    // if the score gets to extreme, stop the tree
    // if (ttl > 4 && (s = get_virtual_score(board)) < -50) {
    //     printf("stop at %d\n", ttl);
    //     return s;
    // }
    int score = color ? 100 : -100;
    move m = init_move(color, 0, 0);
    move best_move = init_move(color, 0, 0);
    int** board_list[64] = {NULL};
    if (color) {
        // color == 1 // RED
        if (board[1][0] > 0) {
            best_move->p1 = 1;
            best_move->end_p1 = 0;
            best_move->p2 = 0;
            int** new_board = is_move_possible(board, best_move);
            if (new_board != NULL)
                score = value_move(new_board, 0, ttl);
            destroy_board(new_board);
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
                int** new_board = is_move_possible(board, m);
                if (new_board != NULL) {
                    if (!is_in_board_list(board_list, new_board)) {
                        s = value_move(new_board, 0, ttl);
                        add_to_board_list(board_list, new_board);
                        if (s < score) {
                            score = s;
                            memcpy(best_move, m, sizeof(struct move));
                        }
                    } else {
                        destroy_board(new_board);
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
            int** new_board = is_move_possible(board, best_move);
            if (new_board != NULL)
                score = value_move(new_board, 1, ttl);
            destroy_board(new_board);
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
                int** new_board = is_move_possible(board, m);
                if (new_board != NULL) {
                    if (!is_in_board_list(board_list, new_board)) {
                        s = value_move(new_board, 1, ttl);
                        add_to_board_list(board_list, new_board);
                        if (s > score) {
                            score = s;
                            memcpy(best_move, m, sizeof(struct move));
                        }
                    } else {
                        destroy_board(new_board);
                    }
                }
            }
        }
    }
    for(int i = 0; i < 64; i++) {
        if (board_list[i])
            destroy_board(board_list[i]);
    }
    if (m)
        free(m);
    if (best_move)
        free(best_move);
    return score;
}

move best_move_for_black(int** board) {
    int best_score = -100;
    move best_move = init_move(0, 0, 0);
    move m = init_move(0, 0, 0);
    int score = 0;
    int** board_list[64] = {NULL};
    for (int i = 0; i < 7; i++) {
        // check if there is a piece at the target row
        if (board[0][i] < 1) {
            continue;
        }
        m->p1 = i;
        m->end_p1 = i + 1;
        // check if there is less than 5 pieces at the arrival row
        if (i < 6 && (board[0][m->end_p1] + board[1][m->end_p1]) > 5) {
            continue;
        }
        for (int j = 0; j < 7; j++) {
            m->p2 = j;
            int** new_board = is_move_possible(board, m);
            if (new_board != NULL) {
                if (!is_in_board_list(board_list, new_board)) {
                    score = value_move(new_board, 1, LEVEL);
                    add_to_board_list(board_list, new_board);
                    printf("move %d %d: %d\n", i, j, score);
                    if (score > best_score) {
                        best_score = score;
                        memcpy(best_move, m, sizeof(struct move));
                    }
                } else {
                    destroy_board(new_board);
                }
            }
        }
    }
    for(int i = 0; i < 64; i++) {
        if (board_list[i])
            destroy_board(board_list[i]);
    }
    if (m)
        free(m);
    printf("best_score: %d\n", best_score);
    return best_move;
}

move interactive_move(int** board) {
    printf("Input move: ");
    int p1, p2;
    scanf("%d %d", &p1, &p2);
    return init_move(1, p1, p2);
}

int main() {

    // int start[2][8] = {
    //     {2, 0, 0, 1, 2, 1, 5, 0},
    //     {2, 3, 1, 1, 3, 1, 1, 0},
    // };
    int** board = create_board();
    board[0][0] = 6;
    board[0][1] = 1;
    board[0][2] = 1;
    board[0][3] = 1;
    board[0][4] = 1;
    board[0][5] = 1;
    board[0][6] = 1;
    board[0][7] = 0;

    board[1][0] = 0;
    board[1][1] = 1;
    board[1][2] = 1;
    board[1][3] = 1;
    board[1][4] = 1;
    board[1][5] = 1;
    board[1][6] = 1;
    board[1][7] = 6;

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
