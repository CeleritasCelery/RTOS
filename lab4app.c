/* 
File: lab4b_app.c
Revision date: 23 December 2003
Description: Application code for EE 425 lab 4B (Kernel essentials B)
*/

#include "clib.h"
#include "yaku.h"

#define ASTACKSIZE 256          /* Size of each stack in words */
#define BSTACKSIZE 256
#define CSTACKSIZE 256

int AStk[ASTACKSIZE];           /* Space for each task's stack */
int BStk[BSTACKSIZE];
int CStk[CSTACKSIZE];

void ATask(void);               /* Function prototypes for task code */
void BTask(void);
void CTask(void);

void main(void)
{
    YKInitialize();
    
    printString("Creating task A...\n");
    YKNewTask(ATask, (void *)&AStk[ASTACKSIZE], 5);
    
    printString("Starting kernel...\n");
    YKRun();
}

void ATask(void)
{
    printString("Task A started!\n");

    printString("Creating task C...\n");
    YKNewTask(CTask, (void *)&CStk[CSTACKSIZE], 2);

    printString("Task A is still running.\n");
    exit(0);
}

void BTask(void)
{
    printString("Task B started! Oh no! Task B wasn't supposed to run.\n");
    exit(0);
}

void CTask(void)
{
	printString("Running Task C.\n");
	YKDelayTask(10000);
	printString("Task C is still running! Oh no! Task C was supposed to stop.\n");
	exit(0);
}

