
#include "yaku.h"
#include "clib.h"

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
	printChar('e');
	
	printLinkedList("ready list before run");
	YKRun();
	printLinkedList("ready list after run");
	YKNewTask(BTask, (void *)&BStk[BSTACKSIZE], 7);
	printLinkedList("ready list after b");
	YKNewTask(CTask, (void *)&CStk[CSTACKSIZE], 2);
	printLinkedList("ready list after c");
	YKNewTask(ATask, (void *)&AStk[ASTACKSIZE], 5);
	printLinkedList("ready list after a");
	printString("should neve reach here\n");

}

void ATask(void)
{
	int i = 0;
    	printString("Task A started!\n");
	
	while (i++ < 10000) {}
	printLinkedList("ready list after");
    	exit(0);
}

void BTask(void)
{
	printString("Task B started! Oh no! Task B wasn't supposed to run.\n");
	exit(0);
}

void CTask(void)
{

	printString("Task C started after ");
	exit(0);
}
