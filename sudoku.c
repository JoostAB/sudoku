#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


#define DEBUG 1
#define GS 9 // GridSize
#define EMPTY 0
#define ROWLETTERS 1

enum State {
  START = 1,
  CREATE = 2,
  DISPLAY = 3,
  SOLVE = 4,
  QUIT = 99
} state = START;

void createSudoku();
void clearField();
void removeDigits(int num);
void shuffle(int arr[], int length);
void printGrid();
void getInput();
bool solve(int x, int y);
bool isValid(int v, int x, int y);
bool usedInRow(int v, int x, int y);
bool usedInCol(int v, int x, int y);
bool usedInBox(int v, int x, int y);
bool restart();
bool loop();

int grid[GS][GS];
int lvl = 5;


int main() {
  
  while (loop()) {}

  return EXIT_SUCCESS;
}

bool loop() {
  if (restart()) {
    createSudoku();
    printGrid();
    getInput();
    return true;
  } else {
    return false;
  }
}

void shuffle(int arr[], int length) {
  for (int i = 0; i < length; i++) {
    int si = rand() % length;
    int tmp = arr[i];
    arr[i] = arr[si];
    arr[si] = tmp;
  }
}

/**
 * @brief Solves the puzzle
 * 
 * Recursive method to solve the puzzle. Finds the first possible number for the current cell. If it finds a number it 
 * goes on to the next cell to check if the puzzle can be solved with this value. If not, it tries the next number.
 * 
 * @param x 
 * @param y 
 * @return int 1 = success, 0 = no solution found
 */
bool solve(int x, int y) {
  int d[] = {1,2,3,4,5,6,7,8,9};
  if (x == GS && y == GS - 1) {
    // reached the end, puzzle solved
    return true;
  }

  if (x >= GS) {
    // End of row, go to next row
    y ++;
    x = 0;
  }

  if (grid[x][y] > EMPTY) {
    // Current field already set, solve next
    return solve(x + 1, y);
  }

  // We got an empty field, now lets find a valid value
  shuffle(d, GS);
  for (int i = 0; i < GS; i++) {
    int v = d[i];
    if (isValid(v,x,y)) {
      // Set this valid value and see if we can solve the puzzle
      grid[x][y] = v;
      if (solve(x + 1,y)) {
        // YES! Did it!
        return true;
      } else {
        // Nope, not a valid value
        grid[x][y] = EMPTY;
      }

    }
  }
  
  // No valid value found for this field, so we have an impossible situation.
  return false;
}

/**
 * @brief Create a Sudoku puzzle
 * 
 */
void createSudoku() {
  // Clear the field
  clearField();
  // Fill with a valid combination of digits
  solve(0,0);
  // Remove some to create a puzzle
  removeDigits(lvl * 10);
}

void clearField() {
  for (int x = 0; x < GS; x++) {
    for (int y = 0; y < GS; y++) {
      grid[x][y] = EMPTY;
    }
  }
}

void removeDigits(int num) {
  int x,y;
  while (num > 0) {
    x = rand() % GS;
    y = rand() % GS;
    if (grid[x][y] != EMPTY) {
      grid[x][y] = EMPTY;
      num --;
    }
  }
}

bool isValid(int v, int x, int y) {
  return (!usedInRow(v,x,y) &&
          !usedInCol(v,x,y) &&
          !usedInBox(v,x,y));
}

bool usedInRow(int v, int x, int y) {
  for (int i = 0; i < GS; i++) {
    if (grid[i][y] == v) return true;
  }
  return false;
}

bool usedInCol(int v, int x, int y) {
  for (int i = 0; i < GS; i++) {
    if (grid[x][i] == v) return true;
  }
  return false;
}

bool usedInBox(int v, int x, int y) {
  int boxSize = GS / 3;
  int xStart = x - x % (boxSize);
  int xEnd = xStart + boxSize;
  int yStart = y - y % (boxSize);
  int yEnd = yStart + boxSize;

  for (int _x = xStart; _x < xEnd; _x++) {
    for (int _y = yStart; _y < yEnd; _y++) {
      if (grid[_x][_y] == v) return true;
    }
  }
  return false;
}

void printGrid() {
  printf("\n   1 2 3   4 5 6   7 8 9 \n");
  printf(" +-------+-------+-------+\n");
  for (int y = 0; y < GS; y++) {
    #ifdef ROWLETTERS
    printf("%c|", y + 97);  // Row letters
    #else
    printf("%d|", y + 1); // Row numbers
    #endif
    
    for (int x = 0; x < GS; x++) {
      if (grid[x][y] == EMPTY) {
        printf("  ");
      } else {
        printf(" %d", grid[x][y]);
      }
      if ((x + 1) % 3 == 0) {
        printf(" |");
      }
    }
    printf("\n");
    if ((y + 1) % 3 == 0) {
      printf(" +-------+-------+-------+\n");
    }
  }
}

void getInput() {
  int x, y, v;
  #ifdef ROWLETTERS
  printf("Enter col, row and value to solve sudoku (e.g., '2 a 6', or '0 0 0' to exit):\n");
  char c;
  while (scanf("%d %c %d", &x, &c, &v) == 3) {
    if (c == '0') {
      y = 0;
    } else {
      if (c >= 'a') {
        y = (c - 'a') + 1;
      } else {
        y = (c - 'A') + 1;
      }
    }
  #else
  printf("Enter row, column and value to solve sudoku (e.g., '2 4 6', or '0 0 0' to exit):\n");
  while (scanf("%d %d %d", &x, &y, &v) == 3) {
  #endif
    if (x == 0 && y == 0 && v == 0) {
      // 0 0 0 ENtered, so try to solve game
      if (solve(0,0) == 1) {
        printGrid();
        printf("SOLVED!!!\n");
      } else {
        printf("No Valid solution found...\n");
      }
      break;
    }

    if (x < 1 || x > GS || y < 1 || y > GS || v < 1 || v > GS) {
      printf("Invalid input. Please enter row, column, and value between 1 and 9:\n");
      continue;
    }

    x--;
    y--;
    if (grid[x][y] != EMPTY) {
      printf("Cannot change given value. Please choose an empty cell:\n");
      continue;
    }
    
    if (!isValid(v,x,y)) {
      printf("Illegal value at this location\n");
      continue;
    }

    grid[x][y] = v;
    printGrid();
  }
}

bool restart() {
  static bool isFirst = true;
  
  if (isFirst) {
    printf("Welcome to Sudoku!\n");
    isFirst = false;
  } else {
    printf("Do you want to play another game? (y/n)\n");
    char c;
    while (scanf("%c", &c) == 1) {
      if (c == 'n' || c == 'N') {
        printf("Ok, maybe next time. Bye!\n");
        return false;
      }
      break;
    }
  }
  
  printf("Enter level 1-5 (1 = easy, 5 = hard)\n");
  char l;
  while (scanf("%c", &l) == 1) {
    if (l < '1' || l > '5') {
      printf("Not a valid level. Please enter a level from 1 to 5\n");
      continue;
    }
    lvl = l - '0';
    break;
  }
  srand(time(NULL));
  return true;
}