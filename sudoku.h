#ifndef __SUDOKU_H__
#define __SUDOKU_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define GS           9    // GridSize
#define EMPTY        0    // Value of an empty cell
int grid[GS][GS];
int lvl = 0;

#include "sudoku-display.h"


#define xytobox(x, y) ((x/3) + (3*(y/3)))

// Enumeration of status machine states
enum State { 
  NOSTATE,
  START,
  STARTING,
  CREATE,
  PLAY,
  SOLVE, 
  SOLVED,
  QUIT} state = START;



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

#endif // __SUDOKU_H__