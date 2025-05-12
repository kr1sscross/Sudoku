#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 16
#define INF 999999

int board[MAX_SIZE][MAX_SIZE];
int original[MAX_SIZE][MAX_SIZE];
int size;
int box_size;
int lives = 5;

time_t start_time, end_time;

void save_game(const char *filename);
void load_game(const char *filename);

void clear_screen() {
    printf("\033[H\033[J");
}

void show_menu() {
    clear_screen();
    printf("=== Sudoku Game ===\n");
    printf("1. Create a new game\n");
    printf("2. Continue game\n");
    printf("3. Instructions\n");
    printf("4. Solve with Simulated Annealing\n");
    printf("0. Exit\n");
    printf("Your choice: ");
}

void select_new_game() {
    clear_screen();
    printf("Select board size:\n");
    printf("1. 4x4\n");
    printf("2. 9x9\n");
    printf("3. 16x16\n");
    printf("0. Return\n");
    printf("Your choice: ");
}

void select_difficulty() {
    clear_screen();
    printf("Select difficulty:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("0. Return\n");
    printf("Your choice: ");
}

void game_option() {
    clear_screen();
    printf("=== Game Options ===\n");
    printf("1. Save game\n");
    printf("2. Save and exit\n");
    printf("3. Exit without saving\n");
    printf("Your choice: ");
}

void show_instruction() {
    clear_screen();
    printf("=== How to Play Sudoku ===\n");
    printf("- Fill the empty cells with numbers.\n");
    printf("- Each number can appear only once per row, column, and block.\n");
    printf("- Input format for moves: row col number (e.g., 1 3 4)\n");
    printf("- Use logic — not guessing — to solve the puzzle.\n");
    printf("- You have 5 lives. Wrong moves reduce your lives.\n");
    printf("- Game ends when board is full or lives = 0.\n");
    printf("Click Enter to return: ");
    getchar();
    getchar();
}

int is_safe(int row, int col, int num) {
    for (int x = 0; x < size; x++) {
        if (board[row][x] == num || board[x][col] == num)
            return 0;
    }

    int startRow = row - row % box_size;
    int startCol = col - col % box_size;

    for (int i = 0; i < box_size; i++) {
        for (int j = 0; j < box_size; j++) {
            if (board[startRow + i][startCol + j] == num)
                return 0;
        }
    }

    return 1;
}

int fill_board(int row, int col) {
    if (row == size - 1 && col == size)
        return 1;
    if (col == size)
        return fill_board(row + 1, 0);
    if (board[row][col] != 0)
        return fill_board(row, col + 1);

    int nums[MAX_SIZE];
    for (int i = 0; i < size; i++) nums[i] = i + 1;

    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }

    for (int i = 0; i < size; i++) {
        if (is_safe(row, col, nums[i])) {
            board[row][col] = nums[i];
            if (fill_board(row, col + 1))
                return 1;
            board[row][col] = 0;
        }
    }
    return 0;
}

void remove_cells(int difficulty) {
    int total_cells = size * size;
    int cells_to_remove;

    switch (difficulty) {
        case 1: cells_to_remove = total_cells * 0.45; break;
        case 2: cells_to_remove = total_cells * 0.35; break;
        case 3: cells_to_remove = total_cells * 0.25; break;
        default: cells_to_remove = total_cells * 0.35; break;
    }

    while (cells_to_remove > 0) {
        int row = rand() % size;
        int col = rand() % size;
        if (board[row][col] != 0) {
            board[row][col] = 0;
            cells_to_remove--;
        }
    }
}

void print_board() {
    printf("\n");
    printf("  +");
    for (int i = 0; i < size; i++) {
        printf("---");
        if ((i + 1) % box_size == 0 && i != size - 1) {
            printf("+");
        }
    }
    printf("+\n");

    for (int i = 0; i < size; i++) {
        printf("  |");
        for (int j = 0; j < size; j++) {
            if (j % box_size == 0 && j != 0) {
                printf("|");
            }

            if (board[i][j] == 0)
                printf(" . ");
            else
                printf("%2d ", board[i][j]);
        }
        printf("|\n");

        if ((i + 1) % box_size == 0 && i < size - 1) {
            printf("  +");
            for (int j = 0; j < size; j++) {
                printf("---");
                if ((j + 1) % box_size == 0 && j != size - 1) {
                    printf("+");
                }
            }
            printf("+\n");
        }
    }

    printf("  +");
    for (int i = 0; i < size; i++) {
        printf("---");
        if ((i + 1) % box_size == 0 && i != size - 1) {
            printf("+");
        }
    }
    printf("+\n");
}

int input_move() {
    int row, col, num;
    printf("Enter your move (row, column, number) or 0 to access game options: ");
    scanf("%d", &row);

    if (row == 0) {
        int option;
        game_option();
        scanf("%d", &option);

        switch (option) {
            case 1:
                save_game("autosave.sav");
                return 0;
            case 2:
                save_game("autosave.sav");
                printf("Exiting to menu...\n");
                sleep(1);
                return 1;
            case 3:
                printf("Exiting to menu...\n");
                sleep(1);
                return 1;
            default:
                printf("Invalid option!\n");
                sleep(1);
                return 0;
        }
    }

    printf("Enter column and number: ");
    scanf("%d %d", &col, &num);
    row--; col--;

    if (row < 0 || row >= size || col < 0 || col >= size || num < 1 || num > size) {
        printf("Invalid input. Please try again.\n");
        sleep(1);
        return 0;
    }

    if (board[row][col] != 0) {
        printf("Cell already filled. Choose an empty cell.\n");
        sleep(1);
        return 0;
    }

    if (is_safe(row, col, num)) {
        board[row][col] = num;
        printf("Move accepted. You placed %d at position (%d, %d).\n", num, row + 1, col + 1);
    } else {
        printf("Invalid move. Try again.\n");
        lives--;
        printf("You lost a life! Remaining lives: %d\n", lives);
    }

    sleep(1);
    return 0;
}

int check_win() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

void game_loop() {
    while (1) {
        clear_screen();

        if (lives <= 0) {
            printf("\nYou ran out of lives! Game over.\n");
            end_time = time(NULL);
            int duration = (int)difftime(end_time, start_time);
            printf("Time taken: %d minutes %d seconds.\n", duration / 60, duration % 60);
            printf("Press Enter to return to the menu.\n");
            getchar();
            getchar();
            return;
        }

        if (check_win()) {
            printf("\nCongratulations! You solved the Sudoku!\n");
            end_time = time(NULL);
            int duration = (int)difftime(end_time, start_time);
            printf("Time taken: %d minutes %d seconds.\n", duration / 60, duration % 60);
            printf("Press Enter to return to the menu.\n");
            getchar();
            getchar();
            return;
        }

        print_board();
        int should_exit = input_move();
        if (should_exit) return;
    }
}

void save_game(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error saving game!\n");
        sleep(1);
        return;
    }

    fwrite(&size, sizeof(int), 1, file);
    fwrite(&box_size, sizeof(int), 1, file);
    fwrite(board, sizeof(int), MAX_SIZE * MAX_SIZE, file);
    fwrite(&lives, sizeof(int), 1, file);

    fclose(file);
    printf("Game saved successfully.\n");
    sleep(1);
}

void load_game(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error loading game! Creating a new game...\n");
        sleep(1);
        return;
    }

    fread(&size, sizeof(int), 1, file);
    fread(&box_size, sizeof(int), 1, file);
    fread(board, sizeof(int), MAX_SIZE * MAX_SIZE, file);
    fread(&lives, sizeof(int), 1, file);

    fclose(file);
    printf("Game loaded successfully.\n");
    sleep(1);
}

//SA solve code
int calculate_cost(int grid[MAX_SIZE][MAX_SIZE]) {
    int cost = 0;
    for (int i = 0; i < size; i++) {
        uint16_t row_mask[MAX_SIZE+1] = {0}, col_mask[MAX_SIZE+1] = {0};
        for (int j = 0; j < size; j++) {
            row_mask[grid[i][j]]++;
            col_mask[grid[j][i]]++;
        }
        for (int k = 1; k <= size; k++) {
            cost += (row_mask[k] > 1 ? row_mask[k] - 1 : 0);
            cost += (col_mask[k] > 1 ? col_mask[k] - 1 : 0);
        }
    }
    return cost;
}

void swap_random_in_block(int grid[MAX_SIZE][MAX_SIZE], int block_row, int block_col) {
    int i1, j1, i2, j2;
    do {
        i1 = rand() % box_size;
        j1 = rand() % box_size;
        i2 = rand() % box_size;
        j2 = rand() % box_size;
    } while ((i1 == i2 && j1 == j2) || original[block_row*box_size+i1][block_col*box_size+j1] || original[block_row*box_size+i2][block_col*box_size+j2]);

    int *a = &grid[block_row*box_size+i1][block_col*box_size+j1];
    int *b = &grid[block_row*box_size+i2][block_col*box_size+j2];
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void initialize_sa_grid(int grid[MAX_SIZE][MAX_SIZE]) {
    memcpy(grid, board, sizeof(board));
    memcpy(original, board, sizeof(board));

    for (int block_row = 0; block_row < box_size; block_row++) {
        for (int block_col = 0; block_col < box_size; block_col++) {
            int present[MAX_SIZE+1] = {0};

            for (int i = 0; i < box_size; i++) {
                for (int j = 0; j < box_size; j++) {
                    int val = grid[block_row*box_size+i][block_col*box_size+j];
                    if (val > 0)
                        present[val] = 1;
                }
            }

            int to_place[MAX_SIZE], idx = 0;
            for (int i = 1; i <= size; i++) {
                if (!present[i]) to_place[idx++] = i;
            }

            for (int i = idx - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = to_place[i];
                to_place[i] = to_place[j];
                to_place[j] = temp;
            }

            int k = 0;
            for (int i = 0; i < box_size; i++) {
                for (int j = 0; j < box_size; j++) {
                    int row = block_row * box_size + i;
                    int col = block_col * box_size + j;
                    if (grid[row][col] == 0)
                        grid[row][col] = to_place[k++];
                }
            }
        }
    }
}

void solve_with_sa() {
    if (size != 9) {
        printf("Simulated Annealing is implemented only for 9x9 Sudoku.\n");
        return;
    }

    int grid[MAX_SIZE][MAX_SIZE];
    initialize_sa_grid(grid);
    memcpy(original, board, sizeof(board));

    double T = 2.5;
    double T_end = 1e-4;
    double alpha = 0.999;
    int max_iter = 300000;

    int current_cost = calculate_cost(grid);
    int best_cost = current_cost;
    int best_grid[MAX_SIZE][MAX_SIZE];
    memcpy(best_grid, grid, sizeof(grid));

    clock_t start_time = clock();
    int iter;
    for (iter = 0; iter < max_iter && T > T_end && best_cost > 0; iter++) {
        int block_row = rand() % box_size;
        int block_col = rand() % box_size;

        int i1, j1, i2, j2;
        int retries = 10;
        do {
            i1 = rand() % box_size;
            j1 = rand() % box_size;
            i2 = rand() % box_size;
            j2 = rand() % box_size;
        } while ((i1 == i2 && j1 == j2 ||
                 original[block_row*box_size+i1][block_col*box_size+j1] ||
                 original[block_row*box_size+i2][block_col*box_size+j2]) && --retries > 0);

        if (retries <= 0) continue;

        int r1 = block_row * box_size + i1;
        int c1 = block_col * box_size + j1;
        int r2 = block_row * box_size + i2;
        int c2 = block_col * box_size + j2;

        int temp = grid[r1][c1];
        grid[r1][c1] = grid[r2][c2];
        grid[r2][c2] = temp;

        int new_cost = calculate_cost(grid);
        int delta = new_cost - current_cost;

        if (delta < 0 || ((double)rand() / RAND_MAX) < exp(-delta / T)) {
            current_cost = new_cost;
            if (current_cost < best_cost) {
                best_cost = current_cost;
                memcpy(best_grid, grid, sizeof(grid));
            }
        } else {
            grid[r2][c2] = grid[r1][c1];
            grid[r1][c1] = temp;
        }

        T *= alpha;
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    memcpy(board, best_grid, sizeof(board));
    print_board();
    printf("Simulated Annealing finished.\n");
    printf("Iterations: %d\n", iter);
    printf("Time: %.3f seconds\n", time_taken);
    printf("Final cost: %d\n", current_cost);
    printf("Best cost: %d\n", best_cost);
    printf("Press Enter to continue.\n");
    getchar(); getchar();
}


int main() {
    srand(time(NULL));
    int choice, difficulty;

    while (1) {
        show_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                select_new_game();
                scanf("%d", &choice);
                if (choice == 0) break;
                else if (choice == 1) size = 4;
                else if (choice == 2) size = 9;
                else if (choice == 3) size = 16;
                else {
                    printf("Invalid size!\n");
                    sleep(1);
                    break;
                }

                box_size = sqrt(size);
                memset(board, 0, sizeof(board));
                fill_board(0, 0);

                select_difficulty();
                scanf("%d", &difficulty);
                remove_cells(difficulty);

                lives = 5;
                start_time = time(NULL);
                game_loop();
                break;

            case 2:
                load_game("autosave.sav");
                start_time = time(NULL);
                game_loop();
                break;

            case 3:
                show_instruction();
                break;

            case 4:
                solve_with_sa();
                break;

            case 0:
                printf("Exiting the game. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice!\n");
                sleep(1);
                break;
        }
    }

    return 0;
}