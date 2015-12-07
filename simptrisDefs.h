/* 
File: lab6defs.h
Revision date: 4 November
Description: Required definitions for EE 425 lab 6 (Message queues)
*/

#define PIECE_STRUCT_SIZE 4

typedef struct {
	int ID;
	int Type;
	int Orientation;
	int Column;

} piece_t;
