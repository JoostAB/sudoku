#include "sudoku-display.h"

/*
void setStatus(const char* s);
void setInfo(const char* i);

void updateScreen();
void doFlash();
void startFlash(char what, char idx, char color, char count);
void stopFlash();
char getColor(int x, int y);
*/

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
        switch (getColor(x,y)) {
          case 'r':
          {
            printf("\e[31m R \e[0m");
            break;
          }
          case 'y':
          {
            printf("\e[33m Y \e[0m");
            break;
          }
          default:
          {
            printf(" %d ", grid[x][y]);
            break;
          }
        }
        
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

char getColor(int x, int y) {
  char color = DEF_COLOR;
  if (flash[4] == DEF_COLOR) return color;

  switch (flash[0]) {
    case 'r':
      if (flash[1] == y) {
        color = flash[4];
        
      }
      break;
    case 'c':
      if (flash[1] == x) {
        color = flash[4];
      }
      break;
    case 'b': 
      if (flash[1] == xytobox(x,y)) {
        color = flash[4];
      }
      break;
    default:
      color = DEF_COLOR;
      break;
  }
  return color;
}

void doFlash() {
  if (flash[0] == '0') return;
  if (flash[3] > 0) {
    if (clock() - flash_tmr > 250) {
      flash_tmr = clock();
      //flash[4] == DEF_COLOR?flash[4] = flash[2]:flash[4] = DEF_COLOR;
      if (flash[4] == DEF_COLOR) {
        flash[4] = flash[2];
      } else {
        flash[4] = DEF_COLOR;
      }
      
      // char txt[LINE_LENGTH - 1];
      // sprintf(txt, "flashes left: %d, going to %c", flash[3], flash[4]);
      // setStatus(txt);
      
      flash[3]--;
      screenChanged = true;
    } 
  } else {
    stopFlash();
  }
}

void startFlash(char what, char idx, char color, char count) {
  if ( (what == 'r' || what == 'c' || what == 'b' ) &&
       ((idx >= 0) && (idx < GS))                   &&
       (color == 'r' || color == 'y' || color == DEF_COLOR)) {
    flash[0] = what;
    flash[1] = idx;
    flash[2] = color;
    flash[3] = count;
  }
  screenChanged = true;
}

void stopFlash() {
  flash[0] = '0';
  flash[2] = DEF_COLOR;
  flash[3] = 0;
  flash[4] = DEF_COLOR;
  screenChanged = true;
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