/* 
File: lab6inth.c
Revision date: 4 November 2009
Description: Sample interrupt handler code for EE 425 lab 6 (Message queues)
*/

#include "yakk.h"
#include "clib.h"
#include "yaku.h"
#include "simptrisDefs.h"

extern YKQ *PieceQPtr; 
extern YKSEM *MoveSemPtr;
extern piece_t PieceArray[];
extern int linesCleared;

extern int NewPieceID;
extern int NewPieceType;
extern int NewPieceOrientation;
extern int NewPieceColumn;

void resetHandler(void)
{
    exit(0);
}

void tickHandler(void)
{
	YKTickHandler();
}	       

void newPieceHandler(void)
{
	static int next = 0;
	YKEnterMutex();
	
	PieceArray[next].ID = NewPieceID;
	PieceArray[next].Type = NewPieceType;
	PieceArray[next].Orientation = NewPieceOrientation;
	PieceArray[next].Column = NewPieceColumn;
	if (YKQPost(PieceQPtr, (void *) &(PieceArray[next])) == 0)
		printString("  PieceISR: queue overflow! \n");
	else if (++next >= PIECEQ_SIZE)
		next = 0;
	//printString("new piece arrived\n");
	YKExitMutex();
}

void recievedHandler(void)
{
	YKEnterMutex();
	YKSemPost(MoveSemPtr);
	YKEnterMutex();
}
