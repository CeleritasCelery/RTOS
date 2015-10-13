#include "yaku.h"
#include "yakk.h"
#include "clib.h"

#define TaskStackSize 256

#define IDLETASK 0
#define IDLE_PRIORITY 10000

void dispatcher(int sp);

int idleTaskStack[TaskStackSize];
int TCBArrayNum = 1;
bool runCalled = false;
int YKCtxSwCount = 0;

void printIntHex(int arg)
{
	char buf[6] = {0,0,0,0,'h','\0'};
	char nibble = (arg >> 12) & 0x0F;
	buf[0] = (nibble > 0x09) ? (nibble - 10) + 'A' : nibble + '0';
	nibble = (arg >> 8) & 0x0F; 
	buf[1] = (nibble > 0x09) ? (nibble - 10) + 'A' : nibble + '0';
	nibble = (arg >> 4) & 0x0F; 
	buf[2] = (nibble > 0x09) ? (nibble - 10) + 'A' : nibble + '0';
	nibble = (arg >> 0) & 0x0F; 
	buf[3] = (nibble > 0x09) ? (nibble - 10) + 'A' : nibble + '0';
	printString(&buf[0]);
}

void printLinkedList(char* string, int list) {
    /*string is user input to know what list is printing
    print certainList  
     i = 0 for readyList
     i = 1 for Delay
     i = 2 for Susp
     i = 3 for Available */
    char* nameOfList;
    char* delayList = "delayList";
    char* readyList = "readyList";
    char* suspList = "SuspendedList";
    char* availList = "AvailableList";
    TCBptr current;
    switch(list)
    {
        case 0:
            current = YKReadyList;    
            nameOfList = readyList;
            break;
        case 1:    
            current = YKDelayList;    
            nameOfList = delayList; 
            break;
        case 2:
            current = YKSuspList;    
            nameOfList = suspList;
            break;
        case 3:    
            current = YKAvailList;    
            nameOfList = availList;
            break;
        default:
            current = YKReadyList;    
            nameOfList = readyList;
            break;

    }
    printNewLine();
    printString("##############\n");
    printString("contents of ");
    printString(nameOfList);
    printNewLine();
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

void deleteFromLinkedList(TCBptr listNode, TCBptr *topOfList, int deletePriority) {
    static int priorityDepth = 0;
	if (listNode == NULL)
		return;

	if (listNode->priority == deletePriority) {    
		if (priorityDepth == 0) {
		    
			listNode->nextTCB->prevTCB = NULL;
			*topOfList = listNode->nextTCB; 
			listNode->nextTCB = NULL;           
		} else {
			listNode->prevTCB->nextTCB = listNode->nextTCB;
			listNode->nextTCB->prevTCB = listNode->prevTCB;
		}
	} else {
		priorityDepth++;
		deleteFromLinkedList(listNode->nextTCB, topOfList, deletePriority);
		priorityDepth--;    
	}
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
	printInt((int)&YKTCBArray[1]);
	printNewLine();
	YKTCBArray[IDLETASK].priority = IDLE_PRIORITY;
	YKTCBArray[IDLETASK].SPtr = &idleTaskStack[TaskStackSize];//256
	YKTCBArray[IDLETASK].state = ready_st; //will be running
	YKTCBArray[IDLETASK].tickDelay = 0;
	YKTCBArray[IDLETASK].prevTCB = NULL;
	YKTCBArray[IDLETASK].nextTCB = NULL;
	//saving the stack frame 
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // flags
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // CS
	*--((int *)YKTCBArray[IDLETASK].SPtr) = (int)idleTask; // IP
	*--((int *)YKTCBArray[IDLETASK].SPtr) = (int)(&idleTaskStack[TaskStackSize]); // bp
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // as
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // bx
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // cx
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // dx
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // si
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // di
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // es
	*--((int *)YKTCBArray[IDLETASK].SPtr) = 0; // ds
	/*printString("idle IP = ");
        printInt((int)idleTask);
	printNewLine();
	printString("idle SP = ");
        printInt((int)YKTCBArray[IDLETASK].SPtr);
	printNewLine();
	printString("idle BP = ");
        printInt((int)&idleTaskStack[TaskStackSize]);
	printNewLine();
	printString("YKready * = ");
        printInt((int)*(int *)YKReadyList);
	printNewLine();
	*/

	readyTask = NULL;
	YKReadyList = &YKTCBArray[IDLETASK];
	YKExitMutex();
}

void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority ){//Creates a new task
 	YKEnterMutex();	
	YKTCBArray[TCBArrayNum].priority = priority;
	YKTCBArray[TCBArrayNum].SPtr = (void *)((int *)taskStack); //(for iret)
	YKTCBArray[TCBArrayNum].state = ready_st; //will be running
	YKTCBArray[TCBArrayNum].tickDelay = 0;
	//YKTCBArray[TCBArrayNum].nextInst = task;
	YKTCBArray[TCBArrayNum].prevTCB = NULL;
	YKTCBArray[TCBArrayNum].nextTCB = NULL;
	//saving the stack frame 
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // flags
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // CS
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = (int)task; // IP
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = (int)((int *)taskStack); // bp
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // as
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // bx
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // cx
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // dx
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // si
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // di
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // es
	*--((int *)YKTCBArray[TCBArrayNum].SPtr) = 0; // ds

	//saveContext();//need to save registers to stack so it is in same state when return
	if (runCalled) {
	//if the top of ready Task list is less priority need to switch the priority
		adjustPriority(YKReadyList, &YKTCBArray[TCBArrayNum], &YKReadyList);
	}
	TCBArrayNum++;
	YKExitMutex();
	if (runCalled) 	YKScheduler();
}


void YKRun(){// Starts actual execution of user code
	int i;
	YKEnterMutex();
	runCalled = true;
	for (i = 1; i < TCBArrayNum; i++) {//start at one since idle task is 0
		adjustPriority(YKReadyList, &YKTCBArray[i], &YKReadyList);
	}
	YKExitMutex();
	YKScheduler();
} 

void YKDelayTask(unsigned int delayCount){// Delays task for specified number of clock ticks
	YKEnterMutex();	
	readyTask->tickDelay += delayCount;
	if (delayCount) {
		deleteFromLinkedList(YKReadyList, &YKReadyList, readyTask->priority);
		adjustPriority(YKDelayList, readyTask, &YKDelayList);
		
	}	
	YKExitMutex();
	YKScheduler();
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
	if(nestedDepth == 0){
		YKScheduler();
	}	
}

void YKScheduler() {// Determines the highest priority ready task
	YKEnterMutex();	
	if (YKReadyList != readyTask) {// readyTask has to be equal to the stack we are on
		//printString("running schedualer \n");
		YKCtxSwCount++;
		previousTask = readyTask;
		readyTask = YKReadyList;

		//printString("YKReadyList = ");
		//printInt((int)YKReadyList);
		//printNewLine();
		//printString("TCB[0] = ");
		//printInt((int)&YKTCBArray[IDLETASK]);
		//printNewLine();
		printString("SP[0] = ");
		printInt((int)YKTCBArray[0].SPtr);
		printNewLine();
		printString("SP[1] = ");
		printInt((int)YKTCBArray[1].SPtr);
		printNewLine();
		printString("SP[2] = ");
		printInt((int)YKTCBArray[2].SPtr);
		printNewLine();
		printString("readyList sp = ");
		printInt((int)YKReadyList->SPtr);
		printNewLine();
		YKDispatcher((int)YKReadyList->SPtr);
		printString("aSP[0] = ");
		printInt((int)YKTCBArray[0].SPtr);
		printNewLine();
		printString("aSP[1] = ");
		printInt((int)YKTCBArray[1].SPtr);
		printNewLine();
		printString("aSP[2] = ");
		printInt((int)YKTCBArray[2].SPtr);
		printNewLine();
	}	
	YKEnterMutex();	
}


void YKTickHandler() {// The kernel's timer tick interrupt handler
	TCBptr iter;
	TCBptr next;
	static tick = 0;
	YKEnterMutex();		
	iter = YKDelayList;
	next = iter->nextTCB;
	printString("tick ");
	printInt(tick);
	printNewLine();
	tick++;
	while (iter != NULL){
		iter->tickDelay--;
		if (iter->tickDelay <= 0) {			
			if (iter->nextTCB != NULL) {
				iter->nextTCB->prevTCB = iter->prevTCB;// reassign next pointer
			}
			if (iter->prevTCB != NULL) {
				iter->prevTCB->nextTCB = iter->nextTCB;// reassign prev pointer
				
			} else {
				YKDelayList = iter->nextTCB;// reassign the head
			}
			next = iter->nextTCB; // assign here because the next pointer will get globbered in the function
			adjustPriority(YKReadyList, iter, &YKReadyList);
		} else {	
			next = iter->nextTCB;
		}
		iter = next;
	}

	YKExitMutex();	
}

int getYKCtxSwCount(){// Global variable tracking context switches
	return YKCtxSwCount;
}

int getYKIdleCount(){// Global variable used by idle task
         return YKIdleCount;
}

void idleTask(void) {
	while(1) {
		printInt(YKIdleCount);
		YKIdleCount++;
		if (YKIdleCount > 10000)
			exit(0);
	}
}
