#include <stdio.h>
#include <stdlib.h>

// Structure to represent a cell in the maze
struct cell
{
    int row;
    int col;
    struct cell *link;
};
typedef struct cell *CELL;

// Structure to represent the maze
struct maze
{
    int rows;
    int cols;
    char **grid; // 2D array to represent the maze
};
typedef struct maze *MAZE;

CELL getcell();                                         // memory allocation for a cell
MAZE createMaze(int rows1, int cols);                   // memory allocation for maze
void initializeMaze(MAZE maze1, char **grid);           // assigning values to the maze
CELL solveMaze(MAZE maze1, int startRow, int startCol); // find the path from start to enc
void displayMaze(MAZE maze1);                           // display the maze

CELL push(CELL front, int row, int col); // push on to the sll
CELL pop(CELL front);                    // pop from the sll
void displaycell(CELL first);            // display sll
void freeList(CELL first);               // dealocation of sll
void freeMaze(MAZE maze1);               // dealocation of maze

int validMove(MAZE maze1, int row, int col, int **visited); // to check of the cell is valid

CELL getcell() // memory allocation of one cell
{
    CELL x;
    x = (CELL)malloc(sizeof(struct cell));
    if (x == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    return x;
}

void freeList(CELL first) // dealocation of sll
{
    CELL cur = first;
    CELL temp;
    while (cur != NULL)
    {
        temp = cur;
        cur = cur->link;
        free(temp);
    }
}
void freeMaze(MAZE maze1) // dealocation of maze
{
    for (int i = 0; i < maze1->rows; i++)
    {
        free(maze1->grid[i]);
    }
    free(maze1->grid);
    free(maze1);
}

CELL push(CELL first, int row, int col) // push on to the sll
{
    CELL temp;
    temp = getcell();
    temp->row = row;
    temp->col = col;
    temp->link = first;
    return temp;
}

CELL pop(CELL first) // pop from the sll
{
    CELL next;
    if (first == NULL)
        return NULL;
    next = first->link;
    free(first);
    return next;
}

void displaycell(CELL first) // display sll
{
    CELL cur;
    cur = first;
    while (cur != NULL)
    {
        printf("%d %d\n", cur->row, cur->col);
        cur = cur->link;
    }
}

MAZE createMaze(int rows, int cols) // memory allocation for maze
{
    MAZE maze1 = (MAZE)malloc(sizeof(struct maze));
    maze1->rows = rows;
    maze1->cols = cols;
    maze1->grid = (char **)malloc(rows * sizeof(char *));
    for (int i = 0; i < rows; i++)
    {
        maze1->grid[i] = (char *)malloc(cols * sizeof(char));
    }
    return maze1;
}

void initializeMaze(MAZE maze1, char **grid) // assigning values to the maze
{
    for (int i = 0; i < maze1->rows; i++)
    {
        for (int j = 0; j < maze1->cols; j++)
        {
            maze1->grid[i][j] = grid[i][j];
        }
    }
}

int validMove(MAZE maze1, int row, int col, int **visited) // to check of the cell is valid
{
    if (row >= 0 && row < maze1->rows && col >= 0 && col < maze1->cols &&
        maze1->grid[row][col] != '#' && visited[row][col] == 0)
        return 1;
    else
        return 0;
}

CELL solveMaze(MAZE maze1, int startRow, int startCol) // find the path from start to end
{
    // 2d array of flags to check whether cell is visited
    int **visited = (int **)malloc(maze1->rows * sizeof(int *));
    for (int i = 0; i < maze1->rows; i++)
    {
        visited[i] = (int *)malloc(maze1->cols * sizeof(int));
        for (int j = 0; j < maze1->cols; j++)
        {
            visited[i][j] = 0;
        }
    }
    int backtrack = 0;

    // Initialize starting position
    int currentRow = startRow;
    int currentCol = startCol;

    // character used for displaying the path
    char ch = '.';

    // Initialize linked list for path
    CELL path = NULL;

    // push the initial position on to the stack and setting its flag as 1
    path = push(path, currentRow, currentCol);
    visited[currentRow][currentCol] = 1;
    maze1->grid[currentRow][currentCol] = 'S';

    // Directions: 0 - up, 1 - right, 2 - down, 3 - left
    // initial direction is right
    int direction = 1;

    // Move until the exit is found
    while (maze1->grid[currentRow][currentCol] != 'E')
    {
        // start looking from the left of current postion
        direction = (direction + 3) % 4;

        // Look for a valid next move
        int found = 0; // flag to see if valid cell was found
        for (int i = 0; i < 4; i++)
        {
            // cell towards the current direction
            int nextRow = currentRow;
            int nextCol = currentCol;
            if (direction == 0)
                nextRow--;
            else if (direction == 1)
                nextCol++;
            else if (direction == 2)
                nextRow++;
            else if (direction == 3)
                nextCol--;

            // Check if the new cell is valid and not visited
            if (validMove(maze1, nextRow, nextCol, visited))
            {
                // if valid, move to the cell
                backtrack = 0;
                currentRow = nextRow;
                currentCol = nextCol;
                found = 1;
                // Mark current cell as visited
                visited[currentRow][currentCol] = 1;
                if (maze1->grid[currentRow][currentCol] != 'E')
                    maze1->grid[currentRow][currentCol] = ch;

                // Push current cell to path
                path = push(path, currentRow, currentCol);
                // printf("%d %d pushed\n", currentRow, currentCol);
                break;
            }
            // Turn right and repeat
            direction = (direction + 1) % 4;
        }

        // If no valid move is found, backtrack
        if (!found)
        {
            if (backtrack == 0) // used to display when back tracking is started
            {
                displayMaze(maze1);
                printf("After backtracking....\n");
            }
            backtrack = 1;
            // change the path back to ' '
            maze1->grid[path->row][path->col] = ' ';

            // printf("popped %d %d\n", path->row, path->col);

            path = pop(path); // Remove the current cell from the path
            if (path == NULL)
                break; // If path is empty, maze cannot be solved

            // change current postion to top of the sll
            currentRow = path->row;
            currentCol = path->col;

            // repeat until end is found
        }
    }

    // Free memory for visited array of flag
    for (int i = 0; i < maze1->rows; i++)
    {
        free(visited[i]);
    }
    free(visited);

    return path; // Return the path to the exit (if found)
}

void displayMaze(MAZE maze1) // display the maze
{
    for (int i = 0; i < maze1->rows; i++)
    {
        for (int j = 0; j < maze1->cols; j++)
        {
            printf("%c ", maze1->grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main()
{
    CELL first = NULL;
    MAZE maze1 = NULL;
    int rows = 21, cols = 21;

    // different mazes with different dimesions
    /*
        15 * 15
        char *grid[] = {
        "###############",
        "#S            #",
        "# #### ###### #",
        "#    # #    # #",
        "#### # # ## # #",
        "#    # #    # #",
        "# ###### ######",
        "#             #",
        "# ###### ######",
        "# #      #    #",
        "# # #### # ## #",
        "# # #    # ## #",
        "# # # #### ## #",
        "#   #        E#",
        "###############"};*/
    /*
        15 * 19
        char *grid[] = {
        "###################",
        "#S        #       #",
        "#E####### # ##### #",
        "# #       #       #",
        "# # ############# #",
        "# #               #",
        "# ############### #",
        "#                 #",
        "# ############### #",
        "# #               #",
        "# # ############# #",
        "# #        #      #",
        "# ######## # #### #",
        "#          #      #",
        "###################"};*/

    char *grid[] = {
        "#####################",
        "#        #          #",
        "# ###### ########## #",
        "# #               # #",
        "# # ########### # # #",
        "# # #         # # # #",
        "# # # ######### # # #",
        "# # #           # # #",
        "# # ############# # #",
        "# #               # #",
        "# ################# #",
        "#                   #",
        "# ################# #",
        "#                   #",
        "# ################# #",
        "# #  E            # #",
        "# # ############# # #",
        "# #               # #",
        "# ############### # #",
        "#                   #",
        "#####################"};

    int startRow = 1, startCol = 1; // starting position modifiable
    // create and initialize maze
    maze1 = createMaze(rows, cols);
    initializeMaze(maze1, grid);

    // display original maze
    printf("Original Maze:\n");
    displayMaze(maze1);

    // solve the maze and store the path as sll in solution
    CELL solution = solveMaze(maze1, startRow, startCol);

    if (solution != NULL) // if solution exists
    {
        printf("\nSolution path:\n");
        displaycell(solution);
        printf("\nFinal Path: \n");
        displayMaze(maze1);
    }
    else // no solution
        printf("Solution not found.\n");

    // dealocation
    freeList(first);
    freeMaze(maze1);
}
