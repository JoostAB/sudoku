#ifndef __SUDOKU_DISPLAY_H__
#define __SUDOKU_DISPLAY_H__

#include "sudoku.h"

#define clear()       printf("\e[H\033[J")           // Clear screen
#define gotoxy(x, y)  printf("\e[%d;%dH", (y), (x))  // Move cursor to location

#define INP_LENGTH   20   // Max length of input command
#define LINE_LENGTH  70   // Max length of text line (info or status)
#define ERR_FLASH    10
#define DEF_COLOR    48  // Default color char '0'


const int statusLine = 1;           // Line number for status text
const int infoLine = 16;            // Line number for info text
const int inputLine = infoLine + 1; // Line number for input line

/* Update flags */
bool inpChanged = false;
bool inpEntered = false;
bool statusChanged = false;
bool infoChanged = false;
bool screenChanged = true;

clock_t flash_tmr;  // Flash timer

// Flash status: 
// - flash[0] = 0 (off), r (row), c (column) or b (block)
// - flash[1] = nr of row, column or block
// - flash[2] = color, r = red, y = yellow
// - flash[3] = nr of flashes left
// - flash[4] = current color (0 = no color, default)
char flash[5] = {'0',0,DEF_COLOR, 0, DEF_COLOR};  	  

const char* doMoveText = "Enter next move in Column-Row-Value format (eg 1a2 or 6g4)";

/* String buffers */
char status[LINE_LENGTH];
char info[LINE_LENGTH];
char input[INP_LENGTH];

int inpPos = 0;

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

/**
 * @brief Handle flashing digits
 * 
 */
void doFlash();

/**
 * @brief Starts a Flash sequence
 * 
 * @param what r(ow), c(olumn), b(ox) or none (0)
 * @param idx index of row, column or box
 * @param color y(yellow) or r(red)
 * @param count nr of flashes, includes off states.
 */
void startFlash(char what, char idx, char color, char count);

/**
 * @brief Stop any currently running flash sequences
 * 
 */
void stopFlash();

/**
 * @brief Get the current color for coordinates. Depending on flash mode
 * 
 * @param x 
 * @param y 
 * @return char r(ow), c(olumn), b(ox) or none (0)
 */
char getColor(int x, int y);

#endif // __SUDOKU_DISPLAY_H__
