#include "tetronimo.h"
#include "tetris.h"


box tet_def[7][3] = 
{
	{ {0,1},{1,0},{1,1} },		//T_BOX, 
	{ {-1,0},{0,-1},{1,-1} },	//T_Z_DEXTRO, 
	{ {1,0},{0,-1},{1,1} },		//T_Z_LEVO,
	{ {0,-1},{0,1},{0,2} },		// T_POLE, 
	{ {-1,0},{1,0},{0,1} },		//T_JUNCTION, 
	{ {0,1},{1,0},{0,2} },		//T_L_DEXTRO, 
	{ {0,1},{-1,0},{0,2} }		//T_L_LEVO
	
};

u8 tet_color[] = {
	ANSI_Yellow,
	ANSI_Red,
	ANSI_Green,
	ANSI_Cyan,
	ANSI_Magenta,
	ANSI_Yellow, /* no orange!*/
	ANSI_Blue
};


tet new_tet(void)
{
	tet t;

	// place in center of top row	
	t.x_origin = N_COLS / 2;
	t.y_origin = 1;

	int n = rand() % 7;

	//printf("Making tet#%d\n",n);

	memcpy(t.offsets, tet_def[n], sizeof(t.offsets));

	t.color = tet_color[n];

	return t;

}


void tet_to_string(tet t)
{
	printf("Tet at (%d,%d)\nOffsets { (%d,%d), (%d,%d), (%d,%d)}\n",
	t.x_origin, t.y_origin,
	t.offsets[0].xo, t.offsets[0].yo,
	t.offsets[1].xo, t.offsets[1].yo,
	t.offsets[2].xo, t.offsets[2].yo);

}
void rotate_tet(RotateDir dir, tet* t)
{
	for(int i = 0; i < 3; i++)
	{
		u8 tmp_x = t->offsets[i].xo;
		if( dir == ROTATE_CCW)
		{
			t->offsets[i].xo = t->offsets[i].yo;
			t->offsets[i].yo = -tmp_x;
		}
		else
		{
			t->offsets[i].xo = -t->offsets[i].yo;
			t->offsets[i].yo = tmp_x;
		}
	}
}

void translate_tet(TranslateDir dir, tet* t)
{	
	switch(dir)
	{
		case TRANSLATE_DOWN:
			t->y_origin += 1;
			break;

		case TRANSLATE_RIGHT:
			t->x_origin += 1;
			break;

		case TRANSLATE_LEFT:
			t->x_origin -= 1;
			break;

		default: /* no valid dir */
			break;
	}
}