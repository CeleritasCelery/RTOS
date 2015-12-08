/* 
File: lab6defs.h
Revision date: 4 November
Description: Required definitions for EE 425 lab 6 (Message queues)
*/

#define PIECEQ_SIZE 10
#define MOVEQ_SIZE 40

#define PIECE_TYPE_STRAIGHT 1
#define PIECE_TYPE_CORNER 0

#define CORNER_BOTTOM_LEFT 0
#define CORNER_BOTTOM_RIGHT 1
#define CORNER_TOP_RIGHT 2
#define CORNER_TOP_LEFT 3

#define STRAIGHT_HORIZONTAL 0
#define STRAIGHT_VERTICAL 1

#define SLIDE_LEFT  0
#define SLIDE_RIGHT 1

#define ROTATE_CCW 0
#define ROTATE_CW  1 
#define ROTATION_EVEN 0

#define METHOD_SLIDE 0
#define METHOD_ROTATE 1

#define MIDDLE_RIGHT 4
#define MIDDLE_LEFT 1

typedef struct {
	int ID;
	char Direction;
	char Method;
} move_t;

typedef struct {
	int ID;
	int Type;
	int Orientation;
	int Column;

} piece_t;
