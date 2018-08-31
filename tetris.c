
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include "tetronimo.h"
#include "tetris.h"

#define _ESC_ \x1b
#define _CSI_ \x1b\x9b
#define _CUU_ A
#define _CUD_ B
#define _CUF_ C
#define _CUB_ D




int board[N_COLS + LEFT_MARGIN + RIGHT_MARGIN ][ N_ROWS + TOP_MARGIN + BOTTOM_MARGIN];

#define SYM_YEL "\x1b[33m"
#define SYM_GRN "\x1b[32m"
#define SYM_RED "\x1b[31m"
#define SYM_B_YEL "\x1b[93m"
#define SYM_B_GRN "\x1b[92m"
#define SYM_B_RED "\x1b[91m"

#define ANSI_RESET "\x1b[m"
#define ANSI_CLEAR_SCREEN "\x1b[2J"
#define ANSI_SET_TITLE "\x1B[\x9D 0 ;"

#define BORDER_COLOR ANSI_White

#define BACKGROUND_COLOR ANSI_Black

// sentinel values to detect edges of gameplay
#define EDGE_RHS (2<<0)
#define EDGE_LHS (2<<1)
#define EDGE_BTM (2<<2)




typedef enum  { TD_DOWN, TD_LEFT, TD_RIGHT, TD_CLOCKWISE, TD_COUNTERCLOCKWISE } Tet_Direction;


int  getkey(void);
void handle_key_press(tet* t, int c);
void render_game(void);
void restore_cursor(void);
void draw_box(u8 x, u8 y, u8 color);
void debug_cell_print(void);
void tet_to_string(tet t);
u8 move_tet( tet* t, Tet_Direction d);

/*
 
*/

int getkey(void) 
{
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= (unsigned)~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

void restore_cursor(void)
{
	printf("\x1b[%d;%dH", 0, 1); // H =  abs pos row;column
	// int width = 2 + ( 5 * N_COLS ) + 2;
	// int height = 1 + ( 3 * N_ROWS ) + 1 +1; // zero backs up 1 line
	
	// printf("\x1b[%dD", width); // cursor left
	// printf("\x1b[%dA", height); // cursor up
	
	
}	



/*
	void initialize_board(void)
	sets sentinel values in the left, right and bottom edges of the board
	to allow edge detection

	board[] also tracks occupied cells; that is, the current tet and all uncleared cells
*/
void initilize_board(void) //N_COLS + LEFT_MARGIN + RIGHT_MARGIN ][ N_ROWS + TOP_MARGIN + BOTTOM_MARGIN]
{
	for(int i = 0; i < N_ROWS + TOP_MARGIN + BOTTOM_MARGIN; i++)
	{
		board[0][i] = EDGE_LHS;
		board[N_COLS + LEFT_MARGIN + RIGHT_MARGIN - 1][i] = EDGE_RHS;
	}
		for(int i = 0; i < N_COLS + LEFT_MARGIN + RIGHT_MARGIN; i++)
	{
		board[i][N_ROWS + TOP_MARGIN + BOTTOM_MARGIN] = EDGE_BTM;
	}
}
/*
	draw_box(u8 x, u8 y)

	draw the fundamental unit of a tetromino
	Each is made up of 2x2 characters

	origin is at top left, ones based
*/
void draw_box(u8 x, u8 y, u8 color)
{
	u8 row = TOP_MARGIN + (1 * y);
	u8 col = LEFT_MARGIN + (2 * x);

	printf("\x1b[%dm", color);

	printf("\x1b[%d;%dH", row, col); // H =  abs pos row;column

	printf( "\u2588\u2588");

	// printf("box at %d,%d\n", xx, yy);
}

/*
	draw_tet(tet t)

	render the piece in play and update the board
*/

void draw_tet(tet t)
{
	// draw box at tet's origin
	u8 xx = t.x_origin;
	u8 yy = t.y_origin;

	draw_box( xx, yy, t.color );
	board[xx][yy] = t.color;

	for(int i = 0; i < 3; i++)
	{
		draw_box( xx + t.offsets[i].xo, yy + t.offsets[i].yo, t.color);
		board[xx + t.offsets[i].xo][yy + t.offsets[i].yo] = t.color;
	}
}

/*
	erase_tet(tet t)

	delete the piece in play and update the board
*/

void erase_tet(tet t)
{
	// draw box at tet's origin
	u8 xx = t.x_origin;
	u8 yy = t.y_origin;

	draw_box( xx, yy, BACKGROUND_COLOR);
	board[xx][yy] = 0;

	for(int i = 0; i < 3; i++)
	{
		draw_box( xx + t.offsets[i].xo, yy + t.offsets[i].yo, BACKGROUND_COLOR);
		board[xx + t.offsets[i].xo][yy + t.offsets[i].yo] = 0;
	}
}

/************************************************

render(int* cells, int width, int height)

DIsplay the game board

************************************************/

void render_game()
{
	restore_cursor();

	// print top art

	//top row
	printf("\x1b[%dm", BORDER_COLOR );
	printf("\u2554");
	for( int col = 0; col < N_COLS; col++){ printf("\u2550\u2550"); }
	printf("\u2557\n");

	//print game field
	for(int row = 0; row < N_ROWS; row++)
	{
		//left wall
		printf( "\u2551" );
		for(int col = 0; col < N_COLS; col++)
		{
			printf("  ");// each tet square 2 chars wide
		}
		// right wall
		printf( "\u2551\n" ); 		
	}
	printf("\u255A");
	for(int col = 0; col < N_COLS; col++){ printf("\u2550\u2550"); }
	printf("\u255D\n");

}

void handle_key_press( tet* t, int key )
{
	switch(key){

		case 0x1B: // escape is followed by two more chars
			getkey();
			getkey();
			break;

		case 'w':
		case 'W':

		move_tet( t , TD_COUNTERCLOCKWISE );
		break;

		case ' ':

		move_tet( t , TD_CLOCKWISE );
		break;

		case 'a':
		case 'A':
		
		move_tet( t , TD_LEFT );

		break;

		case 's':
		case 'S':

		move_tet( t , TD_DOWN);

		break;

		case 'd':
		case 'D':

		move_tet( t , TD_RIGHT );

		break;

		case 'Q':
		case 'q':
		
		exit(0);
		break;

		default:
		printf("Ignoring -%c-\n", key);
		break;

	}//end switch
	// printf("KEY:%d, moves:%d\n",key, n_cells_moved);

}

void _delay(int xsec)
{
	clock_t begin = clock();
	volatile clock_t end = clock();;

	while( end - begin < xsec ){
		
	 end = clock();
	}

}

void player_moves(tet* t, int duration_ticks)
{
	clock_t tick_begin = clock();
	volatile clock_t tick_now = clock();

	int c;

	while( tick_now - tick_begin < duration_ticks)
	{
		c = getkey();
		if( c != EOF)
		{
			handle_key_press( t, c);
		}
		tick_now = clock();
	}

}

/*
	collides( tet t )
	returns the OR-ed values of cells occupied by tet t
*/

int collides( tet t )
{
	int ret = 0;

	ret |= board[t.x_origin][t.y_origin];

	for(int i = 0; i < 3; i++)
	{
		ret |= board[t.x_origin + t.offsets[i].xo][t.y_origin + t.offsets[i].yo];
	}
	return ret;
}

/*
	move_tet( ... )
	inout: tet* t
	in: Tet_Direction

	The new position of the tetronimo t is calculated. 
	If that position is vacant, t is updated to the new position, and zero is returned.
	Otherwise, the return value is the bitwise OR of the contents of any occupied cells.
	Since all color values are greater than 29, this single value can signal a collision
	with any combination of walls or other occupied cells

*/

u8 move_tet( tet* t, Tet_Direction d)
{
	u8 collisions = 0;
	tet t_next;

	printf("\x1b[%dm", ANSI_BrightGreen);
	printf("\x1b[1;0H");
	tet_to_string(*t);

	switch(d)
	{
		case TD_DOWN:
		t_next = translate_tet( TRANSLATE_DOWN, *t );
		break;

		case TD_LEFT:
		t_next = translate_tet( TRANSLATE_LEFT, *t );
		break;

		case TD_RIGHT:
		t_next = translate_tet( TRANSLATE_RIGHT, *t );
		break;

		case TD_CLOCKWISE:
		t_next = rotate_tet( ROTATE_CW, *t );
		break;

		case TD_COUNTERCLOCKWISE:
		t_next = rotate_tet( ROTATE_CCW, *t );
		break;
	}

	//ensure we do not detect self-collision
	erase_tet(*t);
	collisions = collides(t_next);

	if(collisions == 0)
	{
		//destination is empty
		memcpy(t, &t_next, sizeof(tet));
		printf("\x1b[%dm", ANSI_BrightGreen);
		printf("\x1b[3;0HMoved:");tet_to_string(*t);
	}else
	{
		printf("\x1b[%dm", ANSI_BrightGreen);
		printf("\x1b[3;0HCollided: %02x\n", collisions);
	}		
	// restore tet or draw new
	draw_tet(*t);
	
	return collisions;
}

/*

	score_move()
	Test for any completed rows,
	Animate and render removal of any such rows
	return score for clearing row

*/

u32 score_move(void)
{
	//check rows from bottom up until an empty row is found;

	// if board[][] > ANSI_Black
	return 0;
}


int main(int argc, char** argv)
{	
	u32 tick_length = 200;

	u32 score = 0;
	u32 points;

	u8 collisions;

	srand( (unsigned)time(NULL));

	initilize_board();

	tet t = new_tet(); // initally off screen & will 'drop' into visibility

	//render_game();	
	printf("\x1b[%dm", ANSI_BrightGreen);

	int n = 0;
	char spin[] = { '\\','|','/','-'};

	tet t2;
	for(int k = 0; k<10; k++){

		printf("Orig:");
		t = new_tet();
		tet_to_string(t);

		t2 = rotate_tet(ROTATE_CW, t);
		printf("Rotd:");
		tet_to_string(t2);
	}

	// game loop
	for(;;)
	{
		printf("\x1b[%d;%dH%c", 10, 30, spin[n]); 
		n = (n+1)%4;

		player_moves( &t, tick_length);

		// game moves: drop the tet
		collisions = move_tet( &t , TD_DOWN );

		player_moves( &t, tick_length / 10); //last chance

		if( collisions ){
			
			points = score_move();
			score += points;

			if(points)
			{
				//reduce tick_length, perhaps asymptotically
			}
			// we're ready for a new tetronimo
			t = new_tet();
		}
		printf("\x1b[%dm", ANSI_BrightRed);
		printf("NEVER");	}

	// tidy up
	printf(ANSI_RESET);
	printf(ANSI_CLEAR_SCREEN);  

	return 0;	
}
