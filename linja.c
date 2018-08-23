#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "board.h"
#include "move.h"

/*
 * black plays first
 * the board is represented as the position of all the pawns of each color
 * the first row of a board are the black pawns
 * a board is an int[2][8] or int**
 * a board_list is a board[64] or int***
 */

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
    int score = get_score(board) * 10;
    for (int i = 0; i < 4; i++) {
        score = score + board[0][i] * (i - 3);
    }
    for (int i = 4; i < 8; i++) {
        score = score - board[1][i] * (4 - i);
    }
    return score;
}

void free_node_mem(int*** board_list, move m, move best_move, move* move_list) {
    if (board_list) {
        for(int i = 0; i < 64; i++) {
            if (board_list[i])
                destroy_board(board_list[i]);
        }
    }
    if (m)
        free(m);
    if (best_move)
        free(best_move);
    if (move_list) {
        for(int i = 0; i < 64; i++) {
            if (move_list[i])
                free(move_list[i]);
        }
    }
}

int value_move(int** board, int color, int ttl, int alpha, int beta) {
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
    int score = color ? 10000 : -10000;
    move m = malloc(sizeof(struct move));
    move best_move = malloc(sizeof(struct move));
    assert(m);
    assert(best_move);
    m->color = color;
    best_move->color = color;
    int** board_list[64] = {NULL};
    if (color == 0) {
        if (board[0][6] > 0) {
            best_move->p1 = 6;
            best_move->end_p1 = 7;
            best_move->p2 = 0;
            int** new_board = is_move_possible(board, best_move);
            if (new_board != NULL) {
                score = value_move(new_board, 1, ttl, alpha, beta);
                alpha = max(alpha, score);
                if (alpha >= beta) {
                    free_node_mem(board_list, m, best_move, NULL);
                    return score;
                }
            }
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
                        s = value_move(new_board, 1, ttl, alpha, beta);
                        add_to_board_list(board_list, new_board);
                        if (s > score) {
                            score = s;
                            alpha = max(alpha, score);
                            if (alpha >= beta) {
                                free_node_mem(board_list, m, best_move, NULL);
                                return score;
                            }
                            memcpy(best_move, m, sizeof(struct move));
                        }
                    } else {
                        destroy_board(new_board);
                    }
                }
            }
        }
    } else {
        if (board[1][0] > 0) {
            best_move->p1 = 1;
            best_move->end_p1 = 0;
            best_move->p2 = 0;
            int** new_board = is_move_possible(board, best_move);
            if (new_board != NULL) {
                score = value_move(new_board, 0, ttl, alpha, beta);
                beta = min(beta, score);
                if (alpha >= beta) {
                    free_node_mem(board_list, m, best_move, NULL);
                    return score;
                }
            }
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
                        s = value_move(new_board, 0, ttl, alpha, beta);
                        add_to_board_list(board_list, new_board);
                        if (s < score) {
                            score = s;
                            beta = min(beta, score);
                            if (alpha >= beta) {
                                free_node_mem(board_list, m, best_move, NULL);
                                return score;
                            }
                            memcpy(best_move, m, sizeof(struct move));
                        }
                    } else {
                        destroy_board(new_board);
                    }
                }
            }
        }
    }
    free_node_mem(board_list, m, best_move, NULL);
    return score;
}

move best_move_for_black(int** board, int level) {
    int best_score = -100;
    move best_move = malloc(sizeof(struct move));
    move m = malloc(sizeof(struct move));
    m->color = 0;
    best_move->color = 0;
    int score = 0;
    int** board_list[64] = {NULL};
    move move_list[64] = {NULL};
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
                    score = value_move(new_board, 1, level, -10000, 10000);
                    m->score = score;
                    add_to_board_list(board_list, new_board);
                    move mm = malloc(sizeof(struct move));
                    memcpy(mm, m, sizeof(struct move));
                    add_to_move_list(move_list, mm);
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

    // print out best moves
    qsort(move_list, 64, sizeof(move), move_score_cmp);
    printf("best moves:\n");
    for(int i = 0; i < 8; i++) {
        if (move_list[i])
            print_move(move_list[i]);
    }
    free_node_mem(board_list, m, NULL, move_list);
    printf("best_score: %d\n", best_score);
    return best_move;
}

move interactive_move(int** board) {
    printf("Input move: ");
    move m = malloc(sizeof(struct move));
    assert(m);
    m->color = 1;
    scanf("%d %d", &(m->p1), &(m->p2));
    return m;
}

int main(int argc, char* argv[]) {

    int level = 5;
    if (argc == 2) {
        level = atoi(argv[1]);
    }
    printf("level: %d\n", level);

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
    move m = best_move_for_black(board, level);
    assert(m);
    print_move(m);
    make_move(board, m);
    print_board(board);
    printf("score: %d\n", get_score(board));
    return 0;

    /*
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
