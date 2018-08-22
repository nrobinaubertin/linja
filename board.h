int** create_board();
void destroy_board(int** board);
void copy_board(int** new_board, int** board);
int boardcmp(int** b1, int** b2);
void print_board(int** board);
void print_board_list(int*** board_list);
int is_in_board_list(int*** board_list, int** b);
void add_to_board_list(int*** board_list, int** b);
