#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define DEBUG        1
#define GS           9 // GridSize
#define EMPTY        0

#define clear()      printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

const int statusLine = 1;
const int infoLine = 16;
const int inputLine = infoLine + 1;

bool screenNeedsUpdate = true;

enum State { START = 1, CREATE = 2, DISPLAY = 3, SOLVE = 4, QUIT = 99 } state = START;

void createSudoku();
void clearField();
void clearLine(int line);
void removeDigits(int num);
void writeLine(const char *txt, int line);
void shuffle(int arr[], int length);
void printGrid();
void updateScreen();
void getInput();
bool solve(int x, int y);
bool isValid(int v, int x, int y);
bool usedInRow(int v, int x, int y);
bool usedInCol(int v, int x, int y);
bool usedInBox(int v, int x, int y);
bool restart();
void loop();

int grid[GS][GS];
int lvl = 0;

int main() {
  // Force console to UTF-8 codepage to proper display field
  SetConsoleOutputCP(CP_UTF8);

  clear();
  createSudoku();
  void clearField();
  while (state != QUIT) {
    loop();
  }

  return EXIT_SUCCESS;
}

void loop() {
  if (restart()) {
    createSudoku();
    printGrid();
    getInput();

  } else {
    state = QUIT;
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
  int d[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  if (x == GS && y == GS - 1) {
    // reached the end, puzzle solved
    return true;
  }

  if (x >= GS) {
    // End of row, go to next row
    y++;
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
    if (isValid(v, x, y)) {
      // Set this valid value and see if we can solve the puzzle
      grid[x][y] = v;
      if (solve(x + 1, y)) {
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
  solve(0, 0);
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
  int x, y;
  while (num > 0) {
    x = rand() % GS;
    y = rand() % GS;
    if (grid[x][y] != EMPTY) {
      grid[x][y] = EMPTY;
      num--;
    }
  }
}

bool isValid(int v, int x, int y) { return (!usedInRow(v, x, y) && !usedInCol(v, x, y) && !usedInBox(v, x, y)); }

bool usedInRow(int v, int x, int y) {
  for (int i = 0; i < GS; i++) {
    if (grid[i][y] == v)
      return true;
  }
  return false;
}

bool usedInCol(int v, int x, int y) {
  for (int i = 0; i < GS; i++) {
    if (grid[x][i] == v)
      return true;
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
      if (grid[_x][_y] == v)
        return true;
    }
  }
  return false;
}

void writeLine(const char *txt, int line) {
  clearLine(line);
  printf(txt);
}

void clearLine(int line) {
  gotoxy(0, line);
  for (int i = 0; i < 80; i++)
    putchar(' ');
  gotoxy(0, line);
}

void printGrid() {

  gotoxy(1, 2);
  printf("    1  2  3   4  5  6   7  8  9            \n");
  printf("  ╔═════════╤═════════╤═════════╗          \n");
  for (int y = 0; y < GS; y++) {
    printf("%c ║", y + 'A'); // Row letters
    for (int x = 0; x < GS; x++) {
      if (grid[x][y] == EMPTY) {
        printf("   ");
      } else {
        printf(" %d ", grid[x][y]);
      }
      if ((x + 1) % 3 == 0) {
        if ((x + 1) == GS) {
          printf("║");
        } else {
          printf("│");
        }
      }
    }
    printf("          \n");
    if ((y + 1) % 3 == 0) {
      if ((y + 1) == GS) {
        printf("  ╚═════════╧═════════╧═════════╝          \n");
      } else {
        printf("  ╟─────────┼─────────┼─────────╢          \n");
      }
    }
  }
}

void getInput() {
  int x, y, v;
  writeLine("Enter col, row and value to solve sudoku (e.g., '2 a 6', or '0 0 0' to exit):", infoLine);
  clearLine(inputLine);
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
    if (x == 0 && y == 0 && v == 0) {
      // 0 0 0 Entered, so try to solve game
      if (solve(0, 0) == 1) {
        printGrid();
        writeLine("SOLVED!!!", statusLine);
      } else {
        writeLine("No Valid solution found...", statusLine);
      }
      break;
    }

    if (x < 1 || x > GS || y < 1 || y > GS || v < 1 || v > GS) {
      writeLine("Invalid input. Please enter row, column, and value between 1 and 9:", infoLine);
      clearLine(inputLine);
      continue;
    }

    x--;
    y--;
    if (grid[x][y] != EMPTY) {
      writeLine("Cannot change given value. Please choose an empty cell:", infoLine);
      clearLine(inputLine);
      continue;
    }

    if (isValid(v, x, y)) {
      grid[x][y] = v;
      printGrid();
      clearLine(inputLine);
    } else {
      writeLine("Illegal value at this location", infoLine);
      clearLine(inputLine);
    }
  }
}

bool restart() {
  static bool isFirst = true;

  if (isFirst) {
    clearField();
    printGrid();
    gotoxy(1, statusLine);
    printf("Welcome to Sudoku!\n");
    isFirst = false;
  } else {
    writeLine("Do you want to play another game? (y/n)", infoLine);
    clearLine(inputLine);
    char c;
    while (scanf("%c", &c) == 1) {
      gotoxy(1, inputLine);
      if (c == '\n' || c == '\r')
        continue;
      if (c == 'n' || c == 'N') {
        printf("Ok, maybe next time. Bye!\n");
        return false;
      }
      break;
    }
  }

  writeLine("Enter level 1-5 (1 = easy, 5 = hard)", infoLine);
  clearLine(inputLine);
  char l;
  while (scanf("%c", &l) == 1) {
    if (l == '\n' || l == '\r')
      continue;
    if (l < '1' || l > '5') {
      writeLine("Not a valid level. Please enter a level from 1 to 5", infoLine);
      clearLine(inputLine);
      continue;
    }
    lvl = l - '0';
    break;
  }
  srand(time(NULL));
  return true;
}

int main_new() {
  // Force console to UTF-8 codepage to proper display field
  SetConsoleOutputCP(CP_UTF8);

  clear();
  void clearField();
  while (state != QUIT) {
    loop_new();
  }

  return EXIT_SUCCESS;
}

void loop_new() {
  updateScreen();
  switch (state) {
  case START:
    /* code */
    break;
  case CREATE:
  case DISPLAY:
  case SOLVE:
  default:
    break;
  }
}



void updateScreen() {
  static int y = -1;
  static int line = 2;

  if (!screenNeedsUpdate)
    return;

  switch (y) {
  case -1:
    line = 2;
    gotoxy(1, line);
    printf("    1  2  3   4  5  6   7  8  9            \n");
    printf("  ╔═════════╤═════════╤═════════╗          \n");
    line = line + 2;
    break;
  case (GS):
    gotoxy(1, line);
    printf("  ╚═════════╧═════════╧═════════╝          \n");
    screenNeedsUpdate = false;
    break;
  default:
    gotoxy(1, line);
    printf("%c ║", y + 'A'); // Row letters
    for (int x = 0; x < GS; x++) {
      if (grid[x][y] == EMPTY) {
        printf(" · ");
      } else {
        printf(" %d ", grid[x][y]);
      }
      if ((x + 1) % 3 == 0) {
        if ((x + 1) == GS) {
          printf("║");
        } else {
          printf("│");
        }
      }
    }
    printf("          \n");

    line++;

    if (((y + 1) % 3 == 0) && (y != (GS - 1))) {
      printf("  ╟─────────┼─────────┼─────────╢          \n");
      line++;
    }
    break;
  }

  y = screenNeedsUpdate? y+1 : 0;
}