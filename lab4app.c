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
	//printATask();
    YKNewTask(ATask, (void *)&AStk[ASTACKSIZE], 5);
    
    printString("Starting kernel...\n");
	//printATask();
	printString("YKRun...\n");
    YKRun();
}

void ATask(void)
{
    printString("Task A started!\n");
	//printATask();
    printString("Creating task C...\n");
    YKNewTask(CTask, (void *)&CStk[CSTACKSIZE], 2);
printString("return to A from C.\n");
YKNewTask(BTask, (void *)&BStk[CSTACKSIZE], 1);
	//printATask();
    printString("returning to A to B.\n");
    exit(0);
}

void BTask(void)
{
    printString("Running Task B.\n");
	//printATask();
	YKDelayTask(10000);
	printString("Task B is still running! Oh no! Task B was supposed to stop.\n");
	exit(0);
}

void CTask(void)
{
	
	printString("Running Task C.\n");
	//printATask();
	YKDelayTask(10000);
	printString("Task C is still running! Oh no! Task C was supposed to stop.\n");
	exit(0);
}

