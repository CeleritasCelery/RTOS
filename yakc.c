#include "yaku.h"
#include "yakk.h"
#include "clib.h"

#define TaskStackSize 256

#define IDLETASK 0
#define IDLE_PRIORITY 10000
#define ATASK 1
#define BTASK 2
#define CTASK 3
#define DTASK 4
void saveContext();
void dispatcher();

int idleTaskStack[TaskStackSize];
int TCBArrayNum = 1;
bool runSched = false;
bool runCalled = false;

//doesn't need to be atomic
void printLinkedList(char* string) {
	TCBptr head = YKReadyList;
	//string is user input to know what list is printing
	TCBptr current = head;
	printNewLine();
	printString("##############\n");
	printString("contents of ");
	printString(string);
	while (current != NULL) {
		printString("\nPriority = ");
		printInt(current->priority);
		printString(" TickDelay = ");
		printInt(current->tickDelay);
		printString(" State = ");
		printInt(current->state);
		current = current->nextTCB;
	}
	printNewLine();
  	printString("##############\n");
}

void adjustPriority(TCBptr listNode, TCBptr toAdd, TCBptr *topOfList) {
	static int priorityDepth = 0;
	if (listNode == NULL || toAdd == NULL)
		return;
	
	if (listNode->priority > toAdd->priority) {
		toAdd->prevTCB = listNode->prevTCB;
		toAdd->nextTCB = listNode;
		
		if (priorityDepth == 0) {
			listNode->prevTCB = toAdd;
			*topOfList = toAdd;			
		} else {
			listNode->prevTCB->nextTCB = toAdd;
			listNode->prevTCB = toAdd;
		}
	} else {
		priorityDepth++;
		adjustPriority(listNode->nextTCB, toAdd, topOfList);
		priorityDepth--;	
	}
}

void YKInitialize() {// - Initializes all required kernel data structures
	//initialize task one data
	YKEnterMutex();
	YKTCBArray[IDLETASK].priority = IDLE_PRIORITY;
	YKTCBArray[IDLETASK].SPtr = &idleTaskStack[TaskStackSize-2];//256 + 1 - 3 (for iret)
	YKTCBArray[IDLETASK].state = ready_st; //will be running
	YKTCBArray[IDLETASK].tickDelay = 0;
	YKTCBArray[IDLETASK].prevTCB = NULL;
	YKTCBArray[IDLETASK].nextTCB = NULL;
//	currentTask = &YKTCBArray[IDLETASK];
	readyTask = &YKTCBArray[IDLETASK];
	YKReadyList = &YKTCBArray[IDLETASK];
	YKExitMutex();
}

void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority ){//Creates a new task
 	YKEnterMutex();	
	YKTCBArray[TCBArrayNum].priority = priority;
	YKTCBArray[TCBArrayNum].SPtr = (void *)((int *)taskStack); //(for iret)
	YKTCBArray[TCBArrayNum].state = ready_st; //will be running
	YKTCBArray[TCBArrayNum].tickDelay = 0;
	YKTCBArray[TCBArrayNum].nextInst = task;
	YKTCBArray[TCBArrayNum].prevTCB = NULL;
	YKTCBArray[TCBArrayNum].nextTCB = NULL;
	//need to set up the IP, Cs and flags
	*((int *)YKTCBArray[TCBArrayNum].SPtr-3) = *(int *)task;
	*((int *)YKTCBArray[TCBArrayNum].SPtr-2) = 0;//plus 1 because caste to int *
	*((int *)YKTCBArray[TCBArrayNum].SPtr-1) = 0;
	//saveContext();//need to save registers to stack so it is in same state when return
	if (runCalled) {
	//if the top of ready Task list is less priority need to switch the priority
		adjustPriority(YKReadyList, &YKTCBArray[TCBArrayNum], &YKReadyList);
		if (readyTask != YKReadyList) {
			runSched = true;
		}
	}
	TCBArrayNum++;
	YKExitMutex();
}


void YKRun(){// Starts actual execution of user code
	int i;
	YKEnterMutex();
	runCalled = true;
	for(i = 1; i < TCBArrayNum; i++){//start at one since idle task is 0
		adjustPriority(YKReadyList, &YKTCBArray[i], &YKReadyList);
	}
	runSched = true;
	YKExitMutex();
} 

void YKDelayTask(unsigned int delayCount){// Delays task for specified number of clock ticks
	YKEnterMutex();	
	if(readyTask->tickDelay == 0){
		runSched = true;
	}	
	readyTask->tickDelay += delayCount;
	YKExitMutex();
}

void YKEnterMutex(){// Disables interrupts
  asm("cli");
}

void YKExitMutex(){// Enables interrupts
  asm("sti");
}

void YKEnterISR(){// Called on entry to ISR 	
	nestedDepth++;
}

void YKExitISR(){// Called on exit from ISR
 	nestedDepth--;
}

void YKScheduler(){// Determines the highest priority ready task
	if(runSched){	
		runSched = false;
		YKDispatcher();
	}	
}


void YKTickHandler(){// The kernel's timer tick interrupt handler
  //for each of the delayed functions go through and decrement values
  //if(TCB.TickDelay == 0)
  //   addToReadyList();
}

void YKCtxSwCount(){// Global variable tracking context switches

}

void YKIdleCount(){// Global variable used by idle task
  //if(currentTask == idle_task)
         //YKIdleCount++
}

