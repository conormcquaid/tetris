#ifndef __TETRIS_H__
#define __TETRIS_H__

// ANSI COlor palette

#define ANSI_Black     30	
#define ANSI_Red       31	
#define ANSI_Green     32	
#define ANSI_Yellow    33	
#define ANSI_Blue      34	
#define ANSI_Magenta   35	
#define ANSI_Cyan	   36	
#define ANSI_White	   37	
#define ANSI_BrightBlack    90	
#define ANSI_BrightRed 	    91	
#define ANSI_BrightGreen 	92	
#define ANSI_BrightYellow 	93	
#define ANSI_BrightBlue 	94	
#define ANSI_BrightMagenta 	95	
#define ANSI_BrightCyan 	96	
#define ANSI_BrightWhite 	97


// size of borders
// borders extend the game board to allow for edge detection and
// to allow the new piece to move in from offscreen
#define LEFT_MARGIN 1
#define RIGHT_MARGIN 1
#define TOP_MARGIN 5
#define BOTTOM_MARGIN 1

#define N_COLS 10
#define N_ROWS 25

#endif /* __TETRIS_H__*/