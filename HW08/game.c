#include "game.h"
#include "gba.h"
#include <stdio.h>
#include <stdlib.h>
#include "images/flySwatter.h"
#include "images/background.h"
#include "images/black.h"
#include "images/fly.h"
#include "images/swatter.h"

int main(void) {

  REG_DISPCTL = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  //Declaring and initializing score variable
  static int score = 0;

  // Load initial game state
  GBAState state = PRESTART;

  //Setting up the random speeds for the flies
  int deltas[] = { -3, -2, -1, 1, 2, 3 };
  int ndeltas = sizeof(deltas) / sizeof(deltas[0]);

  //This is the game loop
  while (1) {
    currentButtons = BUTTONS;  // Load the current state of the buttons
    ps = cs; //Set the present state to the current state before anything change

    switch (state) {
      
      //state to set up the start screen
      case PRESTART:
        //initialize the title screen and relevant text
        drawFullScreenImageDMA(flySwatter);
        drawString(125, 7, "FLY SWATTER", BLUE);
        drawString(140, 7, "Press ENTER to start", RED);
        state = START;
      break;

      //state to transition to the game state
      case START:
          //Go to the play state if user clicks the start button
          if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
            state = GAME_SETUP;
          }
        break;

      //this is the game state
      case GAME_SETUP:
        //sets up the background and score
        drawFullScreenImageDMA(black);
        score = 0;

        //initialize the state of the flies for the game state
        for (int i = 0; i < MAXFLIES; i++) {
          cs.flies[i].row = randint(0, 160);
          cs.flies[i].col = randint(0, 240);
          cs.flies[i].rd = deltas[randint(0, ndeltas)];
          cs.flies[i].cd = deltas[randint(0, ndeltas)];
        }
        cs.nfly = MAXFLIES;
        
        //initialize the player for the game state
        struct Swatter player;
        player.row = 140;
        player.col = 107;
        player.rd = 1;
        player.cd = 1;
        player.prevCol = 0;
        player.prevRow = 0;
        
        state = PLAY;
        break;

      case PLAY:
          //Reset the program if the select button is pressed
          if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            state = PRESTART;
          }

          //Go to the end game scene when they click B or X on the keyboard
          if (KEY_JUST_PRESSED(BUTTON_B, currentButtons, previousButtons)) {
            state = WIN;
          }

          //If they click up arrow key, move the swatter in the correct direction
          if(KEY_DOWN(BUTTON_UP, currentButtons)) {
            //a check for boundaries
            if (player.prevRow < 0) {
                player.prevRow = player.row;
                player.row = 0;
            } else {
              player.prevRow = player.row;
              player.row -= player.rd;
            }
          }

          //If they click the down arrow key, move the swatter in the correct direction
          if(KEY_DOWN(BUTTON_DOWN, currentButtons)) {
            //checking for boundaries
            if (player.prevRow > 140) {
                player.prevRow = player.row;
                player.row = 140;
            } else {
              player.prevRow = player.row;
              player.row += player.rd;
            }
          }

          //If they click/hold up left key, move the swatter in the correct direction
          if(KEY_DOWN(BUTTON_LEFT, currentButtons)) {
            //a check for boundaries
            if (player.prevCol < 0) {
                player.prevCol = player.col;
                player.col = 0;
            } else {
              player.prevCol = player.col;
              player.col -= player.cd;
            }
          }

          //If they click/hold up right key, move the swatter in the correct direction
          if(KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
            //a check for boundaries
            if (player.prevCol > 220) {
              player.prevCol = player.col;
              player.col = 220;
            } else {
              player.prevCol = player.col;
              player.col += player.cd;
            }
          }
          
          //Update the new position for all the flies
          for (int i = 0; i < cs.nfly; i++) {
              cs.flies[i].row += cs.flies[i].rd;
              cs.flies[i].col += cs.flies[i].cd;
              if(cs.flies[i].row < 0) {
                cs.flies[i].row = 0;
                cs.flies[i].rd = -cs.flies[i].rd;
              }
              if(cs.flies[i].row > 159-11) {
                cs.flies[i].row = 159-11;
                cs.flies[i].rd = -cs.flies[i].rd;
              }
              if(cs.flies[i].col < 0) {
                cs.flies[i].col = 0;
                cs.flies[i].cd = -cs.flies[i].cd;
              }
              if(cs.flies[i].col > 239-11) {
                cs.flies[i].col = 239-11;
                cs.flies[i].cd = -cs.flies[i].cd;
              }
          }

          //if the new positions of the flies clash with
          //the new position of the swatter, then we
          //clear that fly and spawn a new one at a random location
          //lastly, if there is a collision, then increment score
          for (int j = 0; j < cs.nfly; j++) {
            if (collision(cs.flies[j].row, cs.flies[j].col, player.row, player.col) == 1) {
              cs.flies[j].row = randint(0, 160);
              cs.flies[j].col = randint(0, 240);
              cs.flies[j].rd = deltas[randint(0, ndeltas)];
              cs.flies[j].cd = deltas[randint(0, ndeltas)];
              score++;
            }
          }

          //wait for VBlank before drawing any objects
          waitForVBlank();

          //draw rectangles for the flies at the previous position
          for (int j = 0; j < ps.nfly; j++) {
            drawRectDMA(ps.flies[j].row, ps.flies[j].col, 10, 10, BLACK);
          }

          //draw a black rectangle for the swatter
          drawRectDMA(player.prevRow, player.prevCol, 20, 20, BLACK);

          //draw the fly images at the new, updated location
          for (int j = 0; j < cs.nfly; j++) {
            drawImageDMA(cs.flies[j].row, cs.flies[j].col, 10, 10, fly);
          }
          
          //draw the swatter at the new, updated location
          drawImageDMA(player.row, player.col, 20, 20, swatter);

          //draw a black screen for the score
          drawRectDMA(150, 5, 80, 20, BLACK);

          //set up the text for the score and exit instructions
          char buffer[51]; // draw the score
		      sprintf(buffer, "Score: %d", score);
		      drawString(150, 5, buffer, YELLOW);
          drawString(138, 5, "Press X to end", RED);

        break;

      case WIN:
          //initialize the background screen for game ending state
          drawFullScreenImageDMA(background);

          //setup the text for the thank you statement and ending score
          char thanks[51]; 
		      sprintf(thanks, "Thanks! Your score was: %d", score);
          drawString(20, 40, thanks, WHITE);
          drawString(40, 25, "Press DELETE to restart the game", WHITE);
          state = END;
        break;
      
      case END:
        //wait for if the user wants to play again
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            state = PRESTART;
          }
        break;
    }

    previousButtons = currentButtons;  // Store the current state of the buttons
  }
  return 0;
}

//method to detect collisions between the flies and the swatter
int collision(int flyRow, int flyCol, int swatRow, int swatCol) {

	if ((flyCol < (swatCol + 15)) 
		&& (flyCol > swatCol)
		&& ((flyRow + 10) > swatRow)
		&& ((flyRow + 10) < (swatRow + 15))
		) {
		return 1;
	}

	if ((flyCol < (swatCol + 15)) 
		&& (flyCol > swatCol)
		&& (flyRow > swatRow)
		&& (flyRow < (swatRow + 15))
		) {
		return 1;
	}	

  if (((flyCol + 10) > swatCol) && ((flyRow + 10) 
		> swatRow) &&  ((flyCol + 10) < (swatRow + 15)) 
		&& ((flyRow + 10) < (swatCol + 15))) {
		return 1;
	} 

	if (((flyCol + 10) < (swatCol + 15)) 
		&& ((flyCol + 10) > swatCol)
		&& (flyRow > swatRow)
		&& (flyRow < (swatRow + 15))
		) {
		return 1;
	}

  //in the case of no collision, then return 0.
  return 0;
}
