#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "move.h"

void add_to_move_list(move* move_list, move m) {
    int i = 0;
    while(move_list[i] != NULL) {
        i++;
    }
    move_list[i] = m;
}

int move_score_cmp(const void* m1, const void* m2) {
    const move mm1 = *(const move*) m1;
    const move mm2 = *(const move*) m2;
    if (mm1 == NULL)
        return 1;
    if (mm2 == NULL)
        return -1;
    if (mm1->score > mm2->score)
        return -1;
    if (mm1->score < mm2->score)
        return 1;
    return 0;
}

void print_move(move m) {
    printf("%d, %d: %d\n", m->p1, m->p2, m->score);
}

void print_move_list(move* move_list) {
    printf("\n\n______\n\n");
    for(int i = 0; i < 64; i++) {
        if (move_list[i] == NULL) {
            printf("NULL,");
        } else {
            print_move(move_list[i]);
        }
    }
    printf("\n\n______\n\n");
}
