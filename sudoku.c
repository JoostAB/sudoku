#include "sudoku.h"






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
  doFlash();
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



bool isValid(int x, int y, int v) { 
  // return (!usedInRow(x, y, v) && 
  //         !usedInCol(x, y, v) && 
  //         !usedInBox(x, y, v)); }
  return (!usedInBox(x, y, v) && 
          !usedInCol(x, y, v) && 
          !usedInRow(x, y, v)); 
}

bool usedInRow(int x, int y, int v) {
  if (v == EMPTY) return false;
  for (int i = 0; i < GS; i++) {
    if (grid[i][y] == v) {
      if (state == PLAY) startFlash('r', y, 'r', ERR_FLASH);
      return true;
    }
  }
  return false;
}

bool usedInCol(int x, int y, int v) {
  if (v == EMPTY) return false;
  for (int i = 0; i < GS; i++) {
    if (grid[x][i] == v) {
      if (state == PLAY) startFlash('c', x, 'r', ERR_FLASH);
      return true;
    }
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
  //int idx = 0;

  for (int _x = xStart; _x < xEnd; _x++) {
    for (int _y = yStart; _y < yEnd; _y++) {
      if (grid[_x][_y] == v) {
        //idx = xytobox(x, y);
        if (state == PLAY) startFlash('b', xytobox(x, y), 'r', ERR_FLASH);
        return true;
      }
    }
  }
  return false;
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