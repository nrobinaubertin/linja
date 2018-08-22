typedef struct move {
    int color; // 0 = black, 1 = red
    int p1; // phase1 row
    int end_p1; // end row of phase1 move
    int p2;
    int score;
} *move;
move init_move(int color, int p1, int p2);
void add_to_move_list(move* move_list, move m);
int move_score_cmp(const void* m1, const void* m2);
void print_move(move m);
void print_move_list(move* move_list);
