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
unsigned int YKIdleCount = 0;
unsigned YKTickNum = 0;

int idleTaskStack[TaskStackSize];
int TCBArrayNum = 0;
bool runCalled = false;
int YKCtxSwCount = 0;

//semaphores
YKSEM YKSEMArray[MAX_SEMAPHORE];
int semaphoreCount = 0;

//queues
YKQ YKQArray[MAX_QUEUE];
int queueCount = 0;

//events
YKEVENT YKEVENTArray[MAX_EVENT];
int eventCount = 0;

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

void printUVar(char* name, unsigned int var)
{
	printString(name);
	printUInt(var);
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

void printSemList(char* string, YKSEM* sem){
    TCBptr current = sem->pendingList;
    printNewLine();
    printString("##############\n");
    printString(string);
	printNewLine();
	printVar("value = ",sem->value);
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

void printEventList(char* string, YKEVENT* event){
    TCBptr current = event->pendingList;
    printNewLine();
    printString("##############\n");
    printString(string);
	printNewLine();
	printVar("mask = ",event->mask);
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
	//int* sp;
	if (runCalled)
 		YKEnterMutex();	
	YKTCBArray[TCBArrayNum].priority = priority;
	YKTCBArray[TCBArrayNum].SPtr = (void *)((int *)taskStack); //(for iret)
	YKTCBArray[TCBArrayNum].state = ready_st; //will be running
	YKTCBArray[TCBArrayNum].tickDelay = 0;
	YKTCBArray[TCBArrayNum].prevTCB = NULL;
	YKTCBArray[TCBArrayNum].nextTCB = NULL;
	//saving the stack frame 
	//sp = (int *)YKTCBArray[TCBArrayNum].SPtr;
	//*--sp = 0; // flags
	//*--sp = 0; // CS
	//*--sp = (int)task; // IP
	//*--sp = (int)((int *)taskStack); // bp
	//*--sp = 0; // as
	//*--sp = 0; // bx
	//*--sp = 0; // cx
	//*--sp = 0; // dx
	//*--sp = 0; // si
	//*--sp = 0; // di
	//*--sp = 0; // es
	//*--sp = 0; // ds
	//YKTCBArray[TCBArrayNum].SPtr = (void*)sp;

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

void YKEnterISR(){// Called on entry to ISR 	
	nestedDepth++;
}

void YKExitISR(){// Called on exit from ISR
	//printVar("nested depth = ", nestedDepth);
 	nestedDepth--;
	if(nestedDepth == 0){
		YKScheduler();
	}	
}

void YKScheduler() {// Determines the highest priority ready task
	//printString("call schedular\n");
	if (YKReadyList != currentTask) {// currentTask has to be equal to the stack we are on
		//printLinkedList("schedular called", 0);
		YKCtxSwCount++;
		YKDispatcher();
	}	
}


void YKTickHandler() {// The kernel's timer tick interrupt handler
	TCBptr iter;
	TCBptr next;
	
	YKEnterMutex();	
	YKTickNum++;	
	iter = YKDelayList;
	next = iter->nextTCB;
//	printNewLine();
//	printString("Tick ");
//	printUInt(YKTickNum);
//	printNewLine();

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


void idleTask(void) {
	YKExitMutex();	
	while(1) {
		YKEnterMutex();	
		YKIdleCount++;
		YKExitMutex();	
	}
}

/*void YKSemPend(YKSEM* sem)
{
	YKEnterMutex();
	if (sem->value-- <= 0) {
		deleteFromLinkedList(&YKReadyList, currentTask);
		addToLinkedList(&sem->pendingList, currentTask);
		YKScheduler();
	}
	YKExitMutex();
}*/

/*void YKSemPost(YKSEM* sem)
{
	YKEnterMutex();
	sem->value++;	
	if (sem->pendingList) {
		TCBptr task = sem->pendingList;
		deleteFromLinkedList(&sem->pendingList, task);
		addToLinkedList(&YKReadyList, task);
		if (nestedDepth == 0)
			YKScheduler();
	}
	YKExitMutex();
}*/




YKSEM* YKSemCreate(int init)
{
	if (semaphoreCount >= MAX_SEMAPHORE || init < 0)
		return NULL;

	YKSEMArray[semaphoreCount].value = init;
	YKSEMArray[semaphoreCount].pendingList = NULL;
	return &YKSEMArray[semaphoreCount++];
}

//queues

YKQ* YKQCreate(void** start, uint size)
{
	if(queueCount !=  MAX_QUEUE){
		YKQArray[queueCount].baseAddr = start;
		YKQArray[queueCount].size = size;
		YKQArray[queueCount].head = 0;
		YKQArray[queueCount].tail = 0;
		YKQArray[queueCount].elCount = 0;	
		return &YKQArray[queueCount++];	
	} else return NULL;
}

bool YKQEmpty(YKQ* queue) {
	return queue->elCount == 0;
}

bool YKQFull(YKQ* queue) {
	return queue->elCount == queue->size - 1;
}

void* YKQPend(YKQ* queue) {
	void* retVal;
	YKEnterMutex();
	if (YKQEmpty(queue)) {
		deleteFromLinkedList(&YKReadyList, currentTask);
		addToLinkedList(&queue->pendingList, currentTask);
		YKScheduler();
	}
	retVal = queue->baseAddr[queue->head++];
	if (queue->head >= queue->size)
		queue->head = 0;
	queue->elCount--;
	YKExitMutex();
	return retVal;
}

int YKQPost(YKQ* queue, void* msg) {
	int retVal;
	YKEnterMutex();
	if(YKQFull(queue)) {
		retVal = 0;
	} else {
		queue->baseAddr[queue->tail++] = msg;
		if (queue->tail >= queue->size)
			queue->tail = 0;

		queue->elCount++;
		if (queue->pendingList) {
			TCBptr task = queue->pendingList;
			deleteFromLinkedList(&queue->pendingList, task);
			addToLinkedList(&YKReadyList, task);
			if (nestedDepth == 0)
				YKScheduler();
		}
		retVal = 1;
	}
	YKExitMutex();
	return retVal;
}


//Events

YKEVENT* YKEventCreate(uint initialValue)
{
	if (eventCount >= MAX_EVENT)
		return NULL;

	YKEVENTArray[eventCount].mask = initialValue;
	YKEVENTArray[eventCount].pendingList = NULL;
	return &YKEVENTArray[eventCount++];

}

bool eventTriggers(uint pendingFlags, uint setFlags, int waitMode )
{
	if ((waitMode == EVENT_WAIT_ANY) && (pendingFlags & setFlags))
		return true;
	else if ((waitMode == EVENT_WAIT_ALL) && ((pendingFlags & setFlags == pendingFlags)))
		return true;
	else 
		return false;
}

uint YKEventPend(YKEVENT* event, uint eventMask, int waitMode)
{
	YKEnterMutex();
	if (!eventTriggers(eventMask, event->mask, waitMode)) {
		currentTask->eventMask = eventMask;
		currentTask->waitMode = waitMode;
		deleteFromLinkedList(&YKReadyList, currentTask);
		addToLinkedList(&event->pendingList, currentTask);
		YKScheduler();
	}
	YKExitMutex();
	return event->mask;

}
void YKEventSet(YKEVENT* event, uint eventMask)
{
	TCBptr current;
	TCBptr next;
	YKEnterMutex();
	current = event->pendingList;
	next = current->nextTCB;
	event->mask |= eventMask;
	while (current != NULL) {
		next = current->nextTCB;
		if (eventTriggers(current->eventMask, event->mask, current->waitMode)) {	
			deleteFromLinkedList(&event->pendingList, current);
			addToLinkedList(&YKReadyList, current);
		} 
        current = next;
    }
	if (nestedDepth == 0)
		YKScheduler();
	YKExitMutex();
}
void YKEventReset(YKEVENT* event, uint eventMask) 
{
	YKEnterMutex();
	event->mask = event->mask & ~(event->mask & eventMask);
	YKExitMutex();
}


