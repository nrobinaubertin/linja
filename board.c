#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "board.h"

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

void copy_board(int** new_board, int** board) {
    memcpy(new_board[0], board[0], sizeof(int) * 8);
    memcpy(new_board[1], board[1], sizeof(int) * 8);
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
