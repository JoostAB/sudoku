#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define GS           9 // GridSize
#define EMPTY        0
#define INP_LENGTH   20
#define LINE_LENGTH  70

#define clear()      printf("\e[H\033[J")
#define gotoxy(x, y) printf("\e[%d;%dH", (y), (x))

const int statusLine = 1;
const int infoLine = 16;
const int inputLine = infoLine + 1;

enum State { 
  NOSTATE,
  START,
  STARTING,
  CREATE,
  PLAY,
  SOLVE, 
  SOLVED,
  QUIT} state = START;

/*********************/
/* Display functions */
/*********************/

/**
 * @brief Clear a line on the display
 * 
 * Actually fills the line with 80 spaces
 * 
 * @param line 
 */
void clearLine(int line);

/**
 * @brief Set n digits on the field to EMPTY
 * 
 * It will search for n NON-EMPTY fields. So there MUST be at least
 * n fields filled, otherwise this will result in an endless loop!
 * FIXME: See above
 * 
 * @param n 
 */
void removeDigits(int n);

/**
 * @brief Set the Status message. Will be displayed in next loop run
 * 
 * @param s 
 */
void setStatus(const char* s);

/**
 * @brief Set the Info message. Will be displayed in next loop run
 * 
 * @param i 
 */
void setInfo(const char* i);

/**
 * @brief Write a line on the console.
 * 
 * @param txt 
 * @param line 
 */
void writeLine(const char *txt, int line);

/**
 * @brief Full screen update. Everything that is changed will be rewritten to the console.
 * 
 */
void updateScreen();

/******************/
/* Input handling */
/******************/

/**
 * @brief Check if there is anything on the keyboard buffer.
 * If so, handle it. If Enter is pressed, handleInput is called
 * 
 */
void scanInput();

/**
 * @brief Handle any input
 * 
 * 
 */
void handleInput();

/**
 * @brief Clear the input buffer
 * 
 */
void resetInput();

/**************/
/* Game logic */
/**************/

/**
 * @brief Set all cells to EMPTY
 * 
 */
void clearField();

/**
 * @brief Shuffle the elements of an int array
 * 
 * @param arr Pointer to the array
 * @param length Length of the array
 */
void shuffle(int* arr, int length);

/**
 * @brief Solves the puzzle
 *
 * Recursive method to solve the puzzle. Finds the first possible number for the current cell. If it finds a number it
 * goes on to the next cell to check if the puzzle can be solved with this value. If not, it tries the next number.
 *
 * @return true If the puzzle is solved
 * @return false Not possible to solve current situation
 */
bool solvePuzzle();

/**
 * @brief Checks if a given value is valid on the given coordinated
 * 
 * @param x 
 * @param y 
 * @param v - Value
 * @return true or false
 */
bool isValid(int x, int y, int v);

/**
 * @brief Checks if a given value is already used on the current row (y)
 * 
 * @param x 
 * @param y 
 * @param v 
 * @return true or false 
 */
bool usedInRow(int x, int y, int v);

/**
 * @brief Checks if a given value is already used on the current column (x)
 * 
 * @param x 
 * @param y 
 * @param v 
 * @return true or false 
 */
bool usedInCol(int x, int y, int v);

/**
 * @brief Checks if a given value is already used on the current 3x3 box
 * 
 * @param x 
 * @param y 
 * @param v 
 * @return true or false 
 */
bool usedInBox(int x, int y, int v);

/**
 * @brief Checks if all cells are filled
 * 
 * @return true or false 
 */
bool checkForWin();

/**
 * @brief Set the passed value to the passed coordinates, but only if that is a valid move.
 * 
 * @param x 
 * @param y 
 * @param v 
 * @return true Valid move, value set
 * @return false Was not valid
 */
bool setValue(int x, int y, int v);

/**
 * @brief Start the game. Reset field, ask for level and create puzzle
 * 
 */
void startGame();

/**
 * @brief Do the move after all needed checks. Then check for win
 * 
 * @return true Valid move, done
 * @return false Invalid move
 */
bool doMove();

/* Misc */
void loop();

/* Update flags */
bool inpChanged = false;
bool inpEntered = false;
bool statusChanged = false;
bool infoChanged = false;
bool screenChanged = true;

int grid[GS][GS];
int lvl = 0;

const char* doMoveText = "Enter next move in Row-Column-Value (eg 1a2 or 6g4)";

/* String buffers */
char status[LINE_LENGTH];
char info[LINE_LENGTH];
char input[INP_LENGTH];
int inpPos = 0;


int main() {
  // Force console to UTF-8 codepage to proper display field
  SetConsoleOutputCP(CP_UTF8);

  // Clear the screen...
  clear();
  
  // ... and initialize the grid to all empty fields
  clearField();

  setStatus("Welcome to Soduku");

  while (state != QUIT) {
    loop();
  }

  setInfo("Bye bye!!!");
  gotoxy(1,infoLine);
  printf(info);

  return EXIT_SUCCESS;
}

void loop() {
  updateScreen();
  scanInput();
  handleInput();
  switch (state) {
    case START: {
        startGame();
        break;
      }
    case CREATE: {
        if (solvePuzzle()) {
          removeDigits(lvl * 10);
          state = PLAY;
          setInfo(doMoveText);
        }
        break;
      }
    case PLAY:
      // Nothing special here... Just handle input (already done)
      break;
    case SOLVE:
      solvePuzzle();
      break;
    default:
      // Just continue the loop
      break;
  }
}

void shuffle(int* arr, int length) {
  for (int i = 0; i < length; i++) {
    int si = rand() % length;
    int tmp = arr[i];
    arr[i] = arr[si];
    arr[si] = tmp;
  }
}


bool solvePuzzle() {
  static int c = 0;

  int x = c % GS;
  int y = c / GS;
  c++;

  if (c > (GS*GS)) {
    // Puzzle is solved
    // Reset cell counter
    c = 0;
    state = SOLVED;
    return true;
  }

  if (grid[x][y] > EMPTY) {
    // Current field already set, solve next
    return solvePuzzle();
  }

  // We got an empty field, now lets find a valid value
  // Randomize the sequence of digits to test
  int d[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffle(d, GS);

  for (int i = 0; i < GS; i++) {
    int v = d[i];

    // See if we can set the currect digit
    if (!setValue(x,y,v)) continue;

    // It was a valid move, now let's see if we can solve the puzzle with this digit
    if (solvePuzzle()) return true;

    // Nope. Set it to empty and try the next digit
    c--;
    setValue(x,y,EMPTY);  
  }

  // No valid value found for this field, so we have an impossible situation.
  c--;
  return false;
}

// Set all cells to EMPTY
void clearField() {
  for (int x = 0; x < GS; x++) {
    for (int y = 0; y < GS; y++) {
      grid[x][y] = EMPTY;
    }
  }
  screenChanged = true;
}

// Set n cells randomly to EMPTY
void removeDigits(int n) {
  int x, y;
  while (n > 0) {
    x = rand() % GS;
    y = rand() % GS;
    if (grid[x][y] != EMPTY) {
      grid[x][y] = EMPTY;
      n--;
    }
  }
}

bool isValid(int x, int y, int v) { 
  return (!usedInRow(x, y, v) && 
          !usedInCol(x, y, v) && 
          !usedInBox(x, y, v)); }

bool usedInRow(int x, int y, int v) {
  if (v == EMPTY) return false;
  for (int i = 0; i < GS; i++) {
    if (grid[i][y] == v)
      return true;
  }
  return false;
}

bool usedInCol(int x, int y, int v) {
  if (v == EMPTY) return false;
  for (int i = 0; i < GS; i++) {
    if (grid[x][i] == v)
      return true;
  }
  return false;
}

bool usedInBox(int x, int y, int v) {
  if (v == EMPTY) return false;

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
  //printf("\e2K");
}

bool setValue(int x, int y, int v) {
  if (v != EMPTY) {
    if (grid[x][y] != EMPTY) {
      if (state == PLAY) setInfo("Cell is not empty. Choose a free cell.");
      return false;
    }

    if (!isValid(x, y, v)) {
      if (state == PLAY) setInfo("Invalid value for cell. Choose another");
      return false;
    }
  }
    
  grid[x][y] = v;
  screenChanged = true;
  return true;
}

bool checkForWin() {
  for (int x = 0; x < GS; x++) {
    for (int y = 0; y < GS; y++) {
      if (grid[x][y] == EMPTY) return false;
    }
  }
  return true;
}



void updateScreen() {
  static int y = -1;
  static int line = 2;

  if (statusChanged) {
    statusChanged = false;
    gotoxy(1,statusLine);
    printf(status);
  }

  if (infoChanged) {
    infoChanged = false;
    gotoxy(1,infoLine);
    printf(info);
  }

  if (inpChanged) {
    inpChanged = false;
    gotoxy(1,inputLine);
    printf(input);
  }

  if (!screenChanged)
    return;

  // Using color codes. For more info see:
  // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#color-codes
  switch (y) {
  case -1:
    line = 2;
    gotoxy(1, line);
    //printf("\e[1m    1  2  3   4  5  6   7  8  9            \e[0m\n");
    printf("\e[1;33m    1  2  3   4  5  6   7  8  9            \e[0m\n");
    printf("  ╔═════════╤═════════╤═════════╗          \n");
    line = line + 2;
    break;
  case (GS):
    gotoxy(1, line);
    printf("  ╚═════════╧═════════╧═════════╝          \n");
    screenChanged = false;
    line = 2;
    break;
  default:
    gotoxy(1, line);
    printf("\e[1;33m%c\e[0m ║", y + 'A'); // Row letters
    for (int x = 0; x < GS; x++) {
      if (grid[x][y] == EMPTY) {
        printf("\e[2m · \e[0m"); // Dim dot to indicate empty field
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

  switch (line-1) {
    case 5:
      gotoxy(40,line-1);
      printf("Commands:");
      break;
    case 7:
      gotoxy(40,line-1);
      printf("Q or quit");
      break;
    case 8:
      gotoxy(40,line-1);
      printf("S or solve");
      break;
    case 9:
      gotoxy(40,line-1);
      printf("R or restart");
      break;
  }
  y = screenChanged? y+1 : -1;
}

bool isHotKey(char c) {
  switch (c)
  {
  case '\n':
  case '\r':
    inpEntered = true;
    break;
  case 'H':
    setStatus("Hello world");
    break;
  case 'Q':
    state = QUIT;
    break;
  case 'R':
    state = START;
    break;
  default:
    // No hotkey
    return false;
  }
  return true;
}

void scanInput() {
  gotoxy(inpPos + 1, inputLine);
  if (_kbhit()) {
    char ch = _getch();
    if (!isHotKey(ch)) {
      input[inpPos] = ch;
      input[inpPos + 1] = '\0';
      if (inpPos < (INP_LENGTH - 2)) inpPos ++;
    }
    inpChanged = true;
  }
}

void handleInput() {
  if (!inpEntered) return;
  bool handled = false;
  
  if (strcmp(input,"quit") == 0) {
    state = QUIT;
    handled = true;
  } else if (strcmp(input,"solve") == 0) {
    state = SOLVE;
    handled = true;
  } else if (strcmp(input,"restart") == 0) {
    state = START;
    handled = true;
  }

  if (!handled) {
    switch (state) {
      case (STARTING): 
      {
        char i = input[0];
        if (i < 49 || i > 53) {
          setInfo("Illegal value. Please enter a level from 1 - 5.");
          handled = true;
        } else {
          lvl = i - 48;
          handled = true;
          state = CREATE;
        }
        break;
      }
      case (PLAY):
      {
        if (strlen(input) == 3) {
          if ((input[0] >= '1' && input[0] <= '9') &&
              (input[1] >= 'a' && input[1] <= 'i') &&
              (input[2] >= '1' && input[2] <= '9')) {
                doMove();
                handled = true;
              }
        }
        break;
      }
    }
  }

  if (!handled) {
    // Unknown command...
    setInfo("Unknown command at this stage");
    handled = true;
  }

  resetInput();

  return;
}

void resetInput() {
  inpEntered = false;
  input[0] = '\0';
  inpPos = 0;
  clearLine(inputLine);
}

void setStatus(const char* s) {
  memset(status,' ',LINE_LENGTH);
  status[LINE_LENGTH-1] = '\0';
  memcpy(status, s, strlen(s));
  
  statusChanged = true;
}

void setInfo(const char* i) {
  memset(info,' ',LINE_LENGTH);
  info[LINE_LENGTH-1] = '\0';
  memcpy(info, i, strlen(i));
  
  infoChanged = true;
}

void startGame() {
  clearField();
  srand(time(0));
  setInfo("Enter level 1-5 (1 = easy, 5 = hard)");
  state = STARTING;
}

bool doMove() {
  
  if ((strlen(input) == 3) &&
      (input[0] >= '1' && input[0] <= '9') &&
      (input[1] >= 'a' && input[1] <= 'i') &&
      (input[2] >= '1' && input[2] <= '9')) {
        int x = input[0] - '1';
        int y = input[1] - 'a';
        int v = input[2] - '0';
        if (setValue(x,y,v)) {
          if (checkForWin()) {
            setInfo("Yes!!! Solved!");
            state = SOLVED;
          } else {
            setInfo(doMoveText);
          }
          return true;
        }
      }
  return false;
}