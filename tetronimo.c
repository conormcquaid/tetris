#include "tetronimo.h"


box tet_def[7][3] = 
{
	{ {0,1},{1,0},{1,1} },
	{ {-1,0},{0,-1},{1,-1} },
	{ {1,0},{0,-1},{1,1} },
	{ {0,-1},{0,1},{0,2} },
	{ {-1,0},{1,0},{0,1} },
	{ {0,1},{1,0},{0,2} },
	{ {0,1},{-1,0},{0,2} }
	
};

tet new_tet(void)
{
	tet t;
	t.x_origin = 0;
	t.y_origin = 0;

	int n = rand() % 7;

	//printf("Making tet#%d\n",n);

	memcpy(t.offsets, tet_def[n], sizeof(t.offsets));

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
tet rotate_tet(RotateDir dir, tet t)
{
	tet t_new = t;
	for(int i = 0; i < 3; i++)
	{
		u8 tmp_x = t.offsets[i].xo;
		if( dir == ROTATE_CCW)
		{
			t_new.offsets[i].xo = t.offsets[0].yo;
			t_new.offsets[i].yo = -tmp_x;
		}
		else
		{
			t_new.offsets[i].xo = -t.offsets[0].yo;
			t_new.offsets[i].yo = tmp_x;
		}
	}
	return t_new;
}

tet translate_tet(TranslateDir dir, tet t)
{
	tet t_new = t;
	switch(dir)
	{
		case TRANSLATE_DOWN:
			t_new.y_origin += 1;
			break;

		case TRANSLATE_RIGHT:
			t_new.x_origin += 1;
			break;

		case TRANSLATE_LEFT:
			t_new.x_origin -= 1;
			break;

		default: /* no valid dir */
			break;
	}
	return t_new;
}