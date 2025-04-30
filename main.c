#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_SIZE 16

int board[MAX_SIZE][MAX_SIZE];
int size;
int box_size;
int lives = 5;

time_t start_time, end_time;

void save_game(const char *filename);
void load_game(const char *filename);

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// main menu
void show_menu() {
    printf("=== Sudoku Game ===\n");
    printf("1. Create a new game\n");
    printf("2. Continue game\n");
    printf("3. Instructions\n");
    printf("0. Exit\n");
    printf("Your choice: ");
}

void select_new_game() {
    printf("Select board size:\n");
    printf("1. 4x4\n");
    printf("2. 9x9\n");
    printf("3. 16x16\n");
    printf("0. Return\n");
    printf("Your choice: ");
}

// Create a new game - option 1.
void select_difficulty() {
    printf("Select difficulty:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("0. Return\n");
    printf("Your choice: ");
}

// Game options
void game_option() {
    printf("=== Game Options ===\n");
    printf("1. Save game\n");
    printf("2. Save and exit\n");
    printf("3. Exit without saving\n");
    printf("Your choice: ");
}

// Instruction - option 3.
void show_instruction() {
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
                return 1;
            case 3:
                printf("Exiting to menu...\n");
                return 1;
            default:
                printf("Invalid option!\n");
                return 0;
        }
    }

    printf("Enter column and number: ");
    scanf("%d %d", &col, &num);
    row--; col--;

    if (row < 0 || row >= size || col < 0 || col >= size || num < 1 || num > size) {
        printf("Invalid input. Please try again.\n");
        return 0;
    }

    if (board[row][col] != 0) {
        printf("Cell already filled. Choose an empty cell.\n");
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
        return;
    }

    fwrite(&size, sizeof(int), 1, file);
    fwrite(&box_size, sizeof(int), 1, file);
    fwrite(board, sizeof(int), MAX_SIZE * MAX_SIZE, file);
    fwrite(&lives, sizeof(int), 1, file);

    fclose(file);
    printf("Game saved successfully.\n");
}

void load_game(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error loading game! Creating a new game...\n");
        return;
    }

    fread(&size, sizeof(int), 1, file);
    fread(&box_size, sizeof(int), 1, file);
    fread(board, sizeof(int), MAX_SIZE * MAX_SIZE, file);
    fread(&lives, sizeof(int), 1, file);

    fclose(file);
    printf("Game loaded successfully.\n");
}

int main() {
    srand(time(NULL));
    int choice, difficulty;

    while (1) {
        clear_screen();
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

            case 0:
                printf("Exiting the game. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice!\n");
                break;
        }
    }

    return 0;
}