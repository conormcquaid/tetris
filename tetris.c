
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include "tetronimo.h"

#define _ESC_ \x1b
#define _CSI_ \x1b\x9b
#define _CUU_ A
#define _CUD_ B
#define _CUF_ C
#define _CUB_ D


// size of borders
// borders extend the game board to allow for edge detection and
// to allow the new piece to move in from offscreen
#define LEFT_MARGIN 1
#define RIGHT_MARGIN 1
#define TOP_MARGIN 5
#define BOTTOM_MARGIN 1

#define N_COLS 10
#define N_ROWS 25

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

#define BORDER_COLOR "\x1b[37m"

#define EDGE_RHS (2<<0)
#define EDGE_LHS (2<<1)
#define EDGE_BTM (2<<2)




typedef enum  { VK_NONE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_QUIT } valid_key_t;

int g_score;




int  getkey(void);
void handle_key_press(void);
void render(int cells[N_COLS][N_ROWS], int width, int height);
void restore_cursor(void);
void draw_box(u8 x, u8 y);
void debug_cell_print(void);
void tet_to_string(tet t);

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
void draw_box(u8 x, u8 y)
{
	u8 row = TOP_MARGIN + (1 * x);
	u8 col = LEFT_MARGIN + (2 * y);

	printf("\x1b[%d;%dH", row, col); // H =  abs pos row;column

	printf( "\u2588\u2588");

	// printf("box at %d,%d\n", xx, yy);
}

/*
	draw_current_tet(tet t)

	render the piece in play
*/

void draw_current_tet(tet t)
{
	// draw box at tet's origin
	u8 xx = t.x_origin;
	u8 yy = t.y_origin;

	draw_box( xx, yy);

	for(int i = 0; i < 3; i++)
	{
		draw_box( xx + t.offsets[i].xo, yy + t.offsets[i].yo);
	}
}

/************************************************

render(int* cells, int width, int height)

DIsplay the game board

************************************************/

void render(int cells[N_COLS][N_ROWS], int width, int height)
{
	restore_cursor();

	// print top art

	//top row
	printf( BORDER_COLOR "\u2554");
	for(int col = 0; col < N_COLS; col++){ printf("\u2550\u2550"); }
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

	// int row,col,c;
	
	// printf( BORDER_COLOR );
	// printf("\nScore %d\n", g_score);
	
	// //top row
	// printf( BORDER_COLOR "\u2554\u2550");
	// for(col = 0; col < width; col++){ printf("\u2550\u2550\u2550\u2550\u2550"); }
	// printf("\u2550\u2557\n");
	
	// for(row = 0; row < height; row++){
	// 	//left wall
	// 	printf( "\u2551 ");
		
	// 	for(col = 0; col < width; col++){
			
	// 		// cache ccell value & validate
	// 		c = cells[col][row];
	// 		if(c < 0){ c = 0; }
	// 		if(c > MAX_SYMBOL){ c = MAX_SYMBOL; }
			
	// 		printf( "%s", symbols[c][SYM_COLOR] );
	// 		printf("%s", c ? "\u250C\u2500\u2500\u2500\u2510" : "     ");
			
	// 	}
				
	// 	//right wall / left wall
	// 	printf(BORDER_COLOR " \u2551\n\u2551 ");
		
	// 	for(col = 0; col < width; col++){
			
	// 		// cache ccell value & validate
	// 		c = cells[col][row];
	// 		if(c < 0){ c = 0; }
	// 		if(c > MAX_SYMBOL){ c = MAX_SYMBOL; }
			
	// 		char* n = c ? "\u2502" : " ";
	// 		printf( "%s", symbols[c][SYM_COLOR]);
	// 		printf(	"%s%s%s", n, c ? symbols[c][SYM_LEGEND] : "   ", n);
			
	// 	}
				
	// 	//right wall / left wall
	// 	printf(BORDER_COLOR " \u2551\n\u2551 ");
		
	// 	for(col = 0; col < width; col++){
			
	// 		// cache cell value & validate
	// 		c = cells[col][row];
	// 		if(c < 0){ c = 0; }
	// 		if(c > MAX_SYMBOL){ c = MAX_SYMBOL; }
			
	// 		printf( "%s", symbols[c][SYM_COLOR] );
	// 		printf("%s", c ? "\u2514\u2500\u2500\u2500\u2518" : "     ");
			
	// 	}
		
	// 	//right wall : end of row
	// 	printf(BORDER_COLOR " \u2551\n");
	// }
	// //bottom row
	// printf("\u255A\u2550");
	// for(col = 0; col < width; col++){ printf("\u2550\u2550\u2550\u2550\u2550"); }
	// printf("\u2550\u255D");
	
	// restore_cursor();
}

void handle_key_press(void){

	int key, n_cells_moved = 0;

	//
	while( (n_cells_moved == 0) ){

		key = getkey();
		//n_cells_moved = 0;

		switch(key){

			case 0x1B:
			 getkey();
			 getkey();
			 break;

			case 'w':
			case 'W':

			//n_cells_moved = move_up( board );
			break;

			case 'a':
			case 'A':
			//n_cells_moved = move_left( board );
	
			break;

			case 's':
			case 'S':

			//n_cells_moved = move_down( board );
			break;

			case 'd':
			case 'D':
			//n_cells_moved = move_right( board );


			break;

			case 'Q':
			case 'q':
			
			exit(0);
			break;

			default:
		
			n_cells_moved = 0;
			printf("Ignoring -%c-\n", key);
			break;

		}//end switch
		// printf("KEY:%d, moves:%d\n",key, n_cells_moved);
	}//end while
	// printf("KEY exit:%d\n", n_cells_moved);
}

void _delay(int xsec)
{
	clock_t begin = clock();
	volatile clock_t end = clock();;

	while( end - begin < xsec ){
		
	 end = clock();
	}

}


int main(int argc, char** argv)
{	
	volatile int vol;
	//time_t time = time();
	clock_t start = clock();
	//RNG go
	srand( (unsigned)time(NULL));

	// printf( ANSI_RESET);
	// printf( ANSI_CLEAR_SCREEN);

	printf("CPS: %ld \n", CLOCKS_PER_SEC);

	
//	printf("Start time%ld\n", time);
	printf("Start clock %ld\n", start);

	initilize_board();

	// for(int i = 0; i < 10; i++){
	// 	printf("n");
	// 	fflush(stdout);
	// 	_delay(200000);
	// }

	// tet t; // tet in play
	// t.x_origin = 4;
	// t.y_origin = 8;

	// int n = rand() % 7;
	// t.offsets = tet_def[n];
	tet t = new_tet();
	draw_current_tet(t);





	

	//play game
///	for(;;)
//	{
		//render();

		//delay(period);
		usleep(200);

		//drop();

		// slight_delay();

//	}
start = clock();
//time = time();

//	printf("End time%ld\n", time);
	printf("End clock %ld\n", start);



	return 0;	
}
