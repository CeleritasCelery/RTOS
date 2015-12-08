/* 
File: lab7app.c
Revision date: 10 November 2005
Description: Application code for EE 425 lab 7 (Event flags)
*/

#include "clib.h"
#include "yakk.h"                     /* contains kernel definitions */
#include "yaku.h"
#include "simptrisDefs.h"
#include "simptris.h"

#define TASK_STACK_SIZE   512         /* stack size in words */


#define ABS(X) ((X < 0) ? -X : X)

YKEVENT *charEvent;
YKEVENT *numEvent;

//int linesCleared = 0;

piece_t PieceArray[PIECEQ_SIZE];  /* buffers for piece content */
move_t MoveArray[MOVEQ_SIZE];  /* buffers for  MOVE content */
int moveIdx = 0;

int PlaceTaskStk[TASK_STACK_SIZE];     /* a stack for each task */
int ComTaskStk[TASK_STACK_SIZE];
int STaskStk[TASK_STACK_SIZE];

void *PieceQ[PIECEQ_SIZE];           /* space for piece queue */
YKQ *PieceQPtr;                   /* actual name of queue */

void *MoveQ[MOVEQ_SIZE];           /* space for move queue */
YKQ *MoveQPtr;                   /* actual name of queue */

YKSEM *MoveSemPtr;                /* ready to send sem */

#define GET_EVEN_ROTATION(X) (X & 0x02) ? ((X & 0x01) ? 0:1) : X

#define CW_CORNER_ROTATION(X) (X == 0) ? 3 : --X
#define CCW_CORNER_ROTATION(X) (X == 3) ? 0 : ++X


int sideLeftDiff = 0;
int unevenRotation = ROTATION_EVEN; // if negative left side else right. 0 means even
/*
#define PIECE_TYPE_STRAIGHT 1
#define PIECE_TYPE_CORNER 0

#define CORNER_BOTTOM_LEFT 0
#define CORNER_BOTTOM_RIGHT 1
#define CORNER_TOP_RIGHT 2
#define CORNER_TOP_LEFT 3

#define MIDDLE_RIGHT 4
#define MIDDLE_LEFT 1

#define STRAIGHT_HORIZONTAL 0
#define STRAIGHT_VERTICAL 1

#define SLIDE_LEFT  0
#define SLIDE_RIGHT 1

#define ROTATE_CCW 0
#define ROTATE_CW  1 

#define METHOD_SLIDE 0
#define METHOD_ROTATE 1
*/
void createMoveStraight(piece_t* piece, int desiredColumn, int desiredRotation) {
	int i = piece->Column;
	while (i != desiredColumn) { // slide to correct position
		MoveArray[moveIdx].ID = piece->ID;
		MoveArray[moveIdx].Direction = (char)(i < desiredColumn);
		MoveArray[moveIdx].Method = METHOD_SLIDE;
		YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]); //might be an issue later
		i += ((i < desiredColumn) << 1) - 1;//inc or dec
		if (moveIdx >= MOVEQ_SIZE) {
			moveIdx = 0;
		}
	}
	i = piece->Orientation;
	if (i != desiredRotation) { // if rotation is verticle then need to assign to horizontal
		MoveArray[moveIdx].ID = piece->ID;
		MoveArray[moveIdx].Direction = ROTATE_CCW;
		MoveArray[moveIdx].Method = METHOD_ROTATE;
		YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]); //might be an issue later
		if (moveIdx >= MOVEQ_SIZE) {
			moveIdx = 0;
		}
	}
}

void createMoveCorner(piece_t* piece, int desiredColumn, int desiredRotation) {
	int currentCol = piece->Column;
	int currentRotation = piece->Orientation;
	//===MOVE ONE IN===//
	if (currentRotation != desiredRotation)	{	
		if (currentCol >= 5) {
			MoveArray[moveIdx].ID = piece->ID;
			MoveArray[moveIdx].Direction = SLIDE_LEFT;
			MoveArray[moveIdx].Method = METHOD_SLIDE;
			YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
			currentCol--;
		} else if (currentCol <= 0) {
			MoveArray[moveIdx].ID = piece->ID;
			MoveArray[moveIdx].Direction = SLIDE_RIGHT;
			MoveArray[moveIdx].Method = METHOD_SLIDE;
			YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
			currentCol++;
		}

		//===ROTATION LOGIC===//
		switch (currentRotation) {
		case CORNER_BOTTOM_LEFT: 
				while (currentRotation != desiredRotation) {
					currentRotation = CW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
				}   
				break;
		case CORNER_BOTTOM_RIGHT: 
				while (currentRotation != desiredRotation) {
					currentRotation = CCW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CCW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);	
				}   
				break;
		case CORNER_TOP_RIGHT: 
				if (desiredRotation == CORNER_TOP_LEFT){  
					currentRotation = CCW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CCW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
				}else if (desiredRotation == CORNER_BOTTOM_RIGHT) {
					currentRotation = CW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
				}			
				break;
		case CORNER_TOP_LEFT:
				if (desiredRotation == CORNER_TOP_RIGHT) {
					currentRotation = CW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
				}else if (desiredRotation == CORNER_BOTTOM_LEFT){  
					currentRotation = CCW_CORNER_ROTATION(currentRotation);
					MoveArray[moveIdx].ID = piece->ID;
					MoveArray[moveIdx].Direction = ROTATE_CCW;
					MoveArray[moveIdx].Method = METHOD_ROTATE;
					YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
				}
				break;
		default:
			printString("default case!! #bad\n");
		}
	}

	//===MOVE LOGIC===//
	while( currentCol != desiredColumn) {
		MoveArray[moveIdx].ID = piece->ID;
		MoveArray[moveIdx].Direction = (char)(currentCol < desiredColumn);
		MoveArray[moveIdx].Method = METHOD_SLIDE;
		YKQPost(MoveQPtr,(void *)&MoveArray[moveIdx++]);
		currentCol += ((currentCol < desiredColumn) << 1) - 1;//inc or dec
	}
}

#define SIDE_SIZE_THRESHOLD 4
void PlaceTask(void)        /* place the piece */
{
	int desiredRotation = 0;
	int desiredColumn = 0;

	piece_t* currentPiece;
	printString("started place task \n");
	
	while(1) {
		currentPiece = (piece_t *) YKQPend(PieceQPtr); /* get next piece */
		if (currentPiece->Type == PIECE_TYPE_STRAIGHT) {
			if (sideLeftDiff >= SIDE_SIZE_THRESHOLD) { // to the right
				desiredColumn = MIDDLE_RIGHT;		
			} else if (sideLeftDiff <= -SIDE_SIZE_THRESHOLD) {// to the left
				desiredColumn = MIDDLE_LEFT;
			} else  if (unevenRotation == ROTATION_EVEN) { //is even
				desiredColumn = (currentPiece->Column > 2) ? MIDDLE_RIGHT : MIDDLE_LEFT;
			} else { //isn't even
				desiredColumn = (unevenRotation < 0) ? MIDDLE_RIGHT : MIDDLE_LEFT;
			}
			desiredRotation = STRAIGHT_HORIZONTAL;
			sideLeftDiff += (desiredColumn == MIDDLE_LEFT) ? 1 : -1;
			createMoveStraight(currentPiece, desiredColumn, desiredRotation);
		} else { // corner piece
			desiredRotation = GET_EVEN_ROTATION(currentPiece->Orientation);
			if (unevenRotation != ROTATION_EVEN) {
				switch (unevenRotation) {
				case -3: desiredColumn = 0; desiredRotation = CORNER_TOP_LEFT;  break;
				case -2: desiredColumn = 2; desiredRotation = CORNER_TOP_RIGHT; break;
				case  2: desiredColumn = 5; desiredRotation = CORNER_TOP_RIGHT; break;
				case  3: desiredColumn = 3; desiredRotation = CORNER_TOP_LEFT;  break;
				default:
					printString("placeTask: default case!! #bad\n");
				}
				unevenRotation = ROTATION_EVEN;
			} else if (sideLeftDiff >= SIDE_SIZE_THRESHOLD) { // to the right
				desiredColumn  = (desiredRotation == CORNER_BOTTOM_LEFT) ? 3 : 5;
				unevenRotation = (desiredRotation == CORNER_BOTTOM_LEFT) ? CORNER_TOP_RIGHT : CORNER_TOP_LEFT;
				sideLeftDiff -= 2;
			} else if (sideLeftDiff <= -SIDE_SIZE_THRESHOLD) {// to the left
				desiredColumn = (desiredRotation == CORNER_BOTTOM_LEFT) ? 0 : 2;
				unevenRotation = (desiredRotation == CORNER_BOTTOM_LEFT) ? -CORNER_TOP_RIGHT : -CORNER_TOP_LEFT;
				sideLeftDiff += 2;
			} else { //stays on the same side
				if (currentPiece->Column > 2) { // go right
					desiredColumn = (desiredRotation == CORNER_BOTTOM_LEFT) ? 3 : 5;
					unevenRotation = (desiredRotation == CORNER_BOTTOM_LEFT) ? CORNER_TOP_RIGHT : CORNER_TOP_LEFT;
					sideLeftDiff -= 2;
				} else { // go left
					desiredColumn = (desiredRotation == CORNER_BOTTOM_LEFT) ? 0 : 2;
					unevenRotation = (desiredRotation == CORNER_BOTTOM_LEFT) ? -CORNER_TOP_RIGHT : -CORNER_TOP_LEFT;
					sideLeftDiff += 2;
				}	
			}
			createMoveCorner(currentPiece, desiredColumn, desiredRotation);

		}
		
	}

}


void ComTask(void)    /* slide and rotate the piece */
{
	move_t* currentMove;
	printString("started com task \n");
	while (1) {
		YKSemPend(MoveSemPtr); /* wait to send */
		currentMove = (move_t *) YKQPend(MoveQPtr); /* get next move */	
		
		if (currentMove->Method == METHOD_SLIDE) {
			SlidePiece(currentMove->ID, currentMove->Direction);
		} else {
			RotatePiece(currentMove->ID, currentMove->Direction);
		}
	}
}

void STask(void)           /* tracks statistics */
{
    unsigned max, switchCount, idleCount;
    int tmp;

    YKDelayTask(1);
    printString("Welcome to Simptris!\r\n");
    printString("Determining CPU capacity\r\n");
    YKDelayTask(1);
    YKIdleCount = 0;
    YKDelayTask(5);
    max = YKIdleCount / 25;
    YKIdleCount = 0;
	
    YKNewTask(PlaceTask, (void *) &PlaceTaskStk[TASK_STACK_SIZE], 10);
    YKNewTask(ComTask, (void *) &ComTaskStk[TASK_STACK_SIZE], 5);
	SeedSimptris((long)87532);
    StartSimptris();

    while (1)
    {
        YKDelayTask(20);
        
        YKEnterMutex();
        switchCount = YKCtxSwCount;
        idleCount = YKIdleCount;
        YKExitMutex();
        
        printString("<CS: ");
        printInt((int)switchCount);
        printString(", CPU: ");
        tmp = (int) (idleCount/max);
        printInt(100-tmp);
        printString("%>\r\n");
        
        YKEnterMutex();
        YKCtxSwCount = 0;
        YKIdleCount = 0;
        YKExitMutex();
    }
}  

void main(void)
{
    YKInitialize();
	
	PieceQPtr = YKQCreate(PieceQ, PIECEQ_SIZE);
	MoveQPtr = YKQCreate(MoveQ, MOVEQ_SIZE);
	MoveSemPtr = YKSemCreate(1);
	YKNewTask(STask, (void *) &STaskStk[TASK_STACK_SIZE], 0);
    

    YKRun();
}


