#ifndef __TETRONIMO_H__
#define __TETRONIMO_H__

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



typedef enum { ROTATE_CW,ROTATE_CCW }RotateDir;
typedef enum { TRANSLATE_DOWN, TRANSLATE_LEFT, TRANSLATE_RIGHT }TranslateDir;

// each tetronimo is composed of four boxes
// and stored as an (x,y) location on the game board
// along with the offsets for the three remaining other boxes
typedef struct box_struct
{
	i8 xo;
	i8 yo;
}box;

typedef struct tet_type
{

	u8 x_origin;
	u8 y_origin;

	box offsets[3];

	u8 color;

}tet;


// there are seven tetromimos used 
enum { 
	T_BOX, T_POLE, T_JUNCTION, T_Z_LEVO, T_Z_DEXTRO, T_L_LEVO, T_L_DEXTRO
	};

// the offsets describing each of the above
extern box tet_def[7][3];


/*
  funcs
*/

void tet_to_string(tet t);

tet new_tet(void);

tet rotate_tet(RotateDir dir, tet t);

tet translate_tet(TranslateDir dir, tet t);

#endif /* __TETRONIMO_H__*/
