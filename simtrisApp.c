/* 
File: lab7app.c
Revision date: 10 November 2005
Description: Application code for EE 425 lab 7 (Event flags)
*/

#include "clib.h"
#include "yakk.h"                     /* contains kernel definitions */
#include "yaku.h"

#define TASK_STACK_SIZE   512         /* stack size in words */
#define PIECEQ_SIZE 10
#define MOVEQ_SIZE 10

YKEVENT *charEvent;
YKEVENT *numEvent;

int PlaceTaskStk[TASK_STACK_SIZE];     /* a stack for each task */
int ComTaskStk[TASK_STACK_SIZE];

void *PieceQ[PIECEQ_SIZE];           /* space for piece queue */
YKQ *PieceQPtr;                   /* actual name of queue */

void *MoveQ[MOVEQ_SIZE];           /* space for move queue */
YKQ *MoveQPtr;                   /* actual name of queue */



void PlaceTask(void)        /* place the piece */
{

}


void ComTask(void)    /* slide and rotate the piece */
{

}

void main(void)
{
    YKInitialize();
	
	PieceQPtr = YKQCreate(PieceQPtr, PIECEQ_SIZE);
	MoveQPtr = YKQCreate(MoveQPtr, MOVEQ_SIZE);

    YKNewTask(PlaceTask, (void *) &PlaceTaskStk[TASK_STACK_SIZE], 5);
    YKNewTask(ComTask, (void *) &ComTaskStk[TASK_STACK_SIZE], 10);
    
    YKRun();
}
