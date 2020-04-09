#ifndef GAME_H
#define GAME_H

#include "gba.h"

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)
#define OFFSET(r, c, rowlen) ((c) + (rowlen)*(r)) 

//function prototype for the collision method
int collision(int flyRow, int flyCol, int swatRow, int swatCol);

#define MAXFLIES 4

typedef enum {
  PRESTART,
  START,
  GAME_SETUP,
  PLAY,
  WIN,
  END,
} GBAState;

//initialize the fly
struct Fly {
          int row;
          int col;
          int rd;
          int cd;
};

//initiialize the swatter
struct Swatter {
          int row;
          int col;
          int rd;
          int cd;
          int prevRow;
          int prevCol;
};

//initialize the states
struct state {
          int nfly;
          struct Fly flies[MAXFLIES];
} cs, ps;

#endif
