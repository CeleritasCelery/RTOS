/* 
File: lab4b_app.c
Revision date: 23 December 2003
Description: Application code for EE 425 lab 4B (Kernel essentials B)
*/
/*
#include "clib.h"
#include "yaku.h"

#define ASTACKSIZE 256          // Size of each stack in words
#define BSTACKSIZE 256
#define CSTACKSIZE 256

int AStk[ASTACKSIZE];           // Space for each task's stack
int BStk[BSTACKSIZE];
int CStk[CSTACKSIZE];

void ATask(void);               // Function prototypes for task code
void BTask(void);
void CTask(void);

void main(void)
{
    YKInitialize();
    
    printString("Creating task A...\n");
    YKNewTask(ATask, (void *)&AStk[ASTACKSIZE], 5);
    
    printString("Starting kernel...\n");
	printString("YKRun...\n");
    YKRun();
}

void ATask(void)
{
    printString("Task A started!\n");
	//printATask();
    YKNewTask(BTask, (void *)&BStk[ASTACKSIZE], 7);
    printString("delaying task A\n");
 	YKDelayTask(10);
	printString("return to A \n");	
	YKNewTask(CTask, (void *)&CStk[ASTACKSIZE], 3);	
	YKDelayTask(5);
	while(1);	
	exit(0);
}

void BTask(void)
{
    printString("Running Task B.\n");
	//printATask();
	YKDelayTask(10);
	printString("Task B is running!\n");
	YKDelayTask(5);
	while(1);	
	exit(0);
}

void CTask(void)
{
	
	printString("Running Task C.\n");
	//printATask();
	YKDelayTask(15);
	printString("Task C is running! Oh no!\n");
	YKDelayTask(10);
	while(1);	
	exit(0);
}
*/
//File: lab4c_app.c
//Revision date: 23 December 2003
//Description: Application code for EE 425 lab 4C (Kernel essentials C)

/*
#include "clib.h"
#include "yakk.h"

#define STACKSIZE 256          // Size of task's stack in words

int TaskStack[STACKSIZE];      // Space for task's stack

void Task(void);               // Function prototype for task code

void main(void)
{
    YKInitialize();
    
    printString("Creating task...\n");
    YKNewTask(Task, (void *) &TaskStack[STACKSIZE], 0);

    printString("Starting kernel...\n");
    YKRun();
}

void Task(void)
{
    unsigned idleCount;
    unsigned numCtxSwitches;

    printString("Task started.\n");
    while (1)
    {
        printString("Delaying task...\n");

        YKDelayTask(2);

        YKEnterMutex();
        numCtxSwitches = YKCtxSwCount;
        idleCount = YKIdleCount;
        YKIdleCount = 0;
        YKExitMutex();

        printString("Task running after ");
        printUInt(numCtxSwitches);
        printString(" context switches! YKIdleCount is ");
        printUInt(idleCount);
        printString(".\n");
    }
}
*/
/* 
File: lab4d_app.c
Revision date: 23 December 2003
Description: Application code for EE 425 lab 4D (Kernel essentials D)
*/

#include "clib.h"
#include "yakk.h"
#include "yaku.h"

#define ASTACKSIZE 256          /* Size of task's stack in words */
#define BSTACKSIZE 256
#define CSTACKSIZE 256
#define DSTACKSIZE 256

int AStk[ASTACKSIZE];           /* Space for each task's stack  */
int BStk[BSTACKSIZE];
int CStk[CSTACKSIZE];
int DStk[CSTACKSIZE];

void ATask(void);               /* Function prototypes for task code */
void BTask(void);
void CTask(void);
void DTask(void);

void main(void)
{
    YKInitialize();
    
    printString("Creating tasks...\n");
    YKNewTask(ATask, (void *) &AStk[ASTACKSIZE], 3);
    YKNewTask(BTask, (void *) &BStk[BSTACKSIZE], 5);
    YKNewTask(CTask, (void *) &CStk[CSTACKSIZE], 7);
    YKNewTask(DTask, (void *) &DStk[DSTACKSIZE], 8);
    
    printString("Starting kernel...\n");
    YKRun();
}

void ATask(void)
{
    printString("Task A started.\n");
    while (1)
    {
        printString("Task A, delaying 2.\n");
        YKDelayTask(2);
    }
}

void BTask(void)
{
    printString("Task B started.\n");
    while (1)
    {
        printString("Task B, delaying 3.\n");
        YKDelayTask(3);
    }
}

void CTask(void)
{
    printString("Task C started.\n");
    while (1)
    {
        printString("Task C, delaying 5.\n");
        YKDelayTask(5);
    }
}

void DTask(void)
{
    printString("Task D started.\n");
    while (1)
    {
        printString("Task D, delaying 10.\n");
        YKDelayTask(10);
    }
}

