#include "yaku.h"
#include "yakk.h"
#include "clib.h"

#define TaskStackSize 256

#define IDLETASK 0
#define IDLE_PRIORITY 101

void YKDispatcher();

TCBptr currentTask;
TCBptr YKReadyList;//linked list of ready tasks
TCBptr YKSuspList;//linked list of Suspended tasks
TCBptr YKDelayList;//linked list of delayed tasks
TCBptr YKAvailList;//list of available tasks 
TCB YKTCBArray[TASKNUMBER+1];
int nestedDepth = 0;
int YKIdleCount = 0;

int idleTaskStack[TaskStackSize];
int TCBArrayNum = 0;
bool runCalled = false;
int YKCtxSwCount = 0;

//semaphores
YKSEM YKSEMArray[MAX_SEMAPHORE];
int semaphoreCount = 0;

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

void printVar(char* name, int var)
{
	printString(name);
	printInt(var);
    printNewLine();	
}

void printVarHex(char* name, int var)
{
	printString(name);
	printIntHex(var);
    printNewLine();	
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

void deleteFromLinkedList_recurse(TCBptr listNode, TCBptr *topOfList, TCBptr toDelete) {
    static int priorityDepth = 0;
	if (listNode == NULL)
		return;

	if (listNode == toDelete) {    
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
		deleteFromLinkedList_recurse(listNode->nextTCB, topOfList, toDelete);
		priorityDepth--;    
	}
}
void deleteFromLinkedList(TCBptr *List, TCBptr toDelete) {
	deleteFromLinkedList_recurse(*List, List, toDelete);
}


void addToLinkedList_recurse(TCBptr listNode, TCBptr toAdd, TCBptr *topOfList) {
	static int priorityDepth = 0;
	if (toAdd == NULL)
		return;

	if (listNode == NULL && priorityDepth == 0) {
		toAdd->nextTCB = NULL;
		toAdd->prevTCB = NULL;
		*topOfList = toAdd;	
		return;
	}
	
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
	} else if (listNode->nextTCB == NULL){
		listNode->nextTCB = toAdd;
		toAdd->prevTCB = listNode;
	}	else {
		priorityDepth++;
		addToLinkedList_recurse(listNode->nextTCB, toAdd, topOfList);
		priorityDepth--;	
	}
}

void addToLinkedList(TCBptr *List, TCBptr toAdd) {
	addToLinkedList_recurse(*List, toAdd, List);
}

void YKInitialize() {// - Initializes all required kernel data structures
	YKEnterMutex();
	//initialize task one data
	currentTask = NULL;
	YKReadyList = &YKTCBArray[IDLETASK];
	YKNewTask(idleTask, (void *)&idleTaskStack[TaskStackSize], IDLE_PRIORITY); 
}

void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority ){//Creates a new task
	if (runCalled)
 		YKEnterMutex();	
	YKTCBArray[TCBArrayNum].priority = priority;
	YKTCBArray[TCBArrayNum].SPtr = (void *)((int *)taskStack); //(for iret)
	YKTCBArray[TCBArrayNum].state = ready_st; //will be running
	YKTCBArray[TCBArrayNum].tickDelay = 0;
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

	if (runCalled) {
	//if the top of ready Task list is less priority need to switch the priority
		addToLinkedList(&YKReadyList, &YKTCBArray[TCBArrayNum]);
	}
	TCBArrayNum++;
	
	if (runCalled) {
		
		YKScheduler();
		YKExitMutex();
	}
}


void YKRun(){// Starts actual execution of user code
	int i;
	YKEnterMutex();
	runCalled = true;
	for (i = 1; i < TCBArrayNum; i++) {//start at one since idle task is 0
		addToLinkedList(&YKReadyList, &YKTCBArray[i]);
	}
	
	YKScheduler();
	YKExitMutex();
} 

void YKDelayTask(unsigned int delayCount){// Delays task for specified number of clock ticks
	YKEnterMutex();	
	currentTask->tickDelay = delayCount;
	if (delayCount) {
		deleteFromLinkedList(&YKReadyList, currentTask);
		addToLinkedList(&YKDelayList, currentTask);
	}	
	
	YKScheduler();
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
	if(nestedDepth == 0){
		YKScheduler();
	}	
}

void YKScheduler() {// Determines the highest priority ready task
	if (YKReadyList != currentTask) {// currentTask has to be equal to the stack we are on
		YKCtxSwCount++;
		YKDispatcher();
	}	
}


void YKTickHandler() {// The kernel's timer tick interrupt handler
	TCBptr iter;
	TCBptr next;
	static unsigned int tick = 1;
	
	YKEnterMutex();		
	iter = YKDelayList;
	next = iter->nextTCB;
	printNewLine();
	printString("Tick ");
	printUInt(tick);
	printNewLine();

	tick++;
	while (iter != NULL){
		iter->tickDelay--;
		next = iter->nextTCB; // assign here because the next pointer will get globbered if branch taken	
		if (iter->tickDelay <= 0) {
			deleteFromLinkedList(&YKDelayList, iter);
			addToLinkedList(&YKReadyList, iter);
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
	YKExitMutex();	
	while(1) {
		YKIdleCount++;
	}
}

void YKSemPend(YKSEM* sem)
{
	YKEnterMutex();
	if (sem->value-- <= 0) {
		deleteFromLinkedList(&YKReadyList, currentTask);
		addToLinkedList(&sem->pendingList, currentTask);
		YKScheduler(); // can switch context here?
	}
	YKExitMutex();
}

void YKSemPost(YKSEM* sem)
{
	YKEnterMutex();
	sem->value++;	
	if (sem->pendingList) {// will be null if empty?
		TCBptr task = sem->pendingList;
		deleteFromLinkedList(&sem->pendingList, task); // right thing to remove?
		addToLinkedList(&YKReadyList, task); 
		YKScheduler();
	}
	YKExitMutex();
}


YKSEM* YKSemCreate(int init)
{
	if (semaphoreCount >= MAX_SEMAPHORE || init < 0)
		return NULL;

	YKSEMArray[semaphoreCount].value = init;
	YKSEMArray[semaphoreCount].pendingList = NULL;
	semaphoreCount++;
	return &YKSEMArray[semaphoreCount];
}
