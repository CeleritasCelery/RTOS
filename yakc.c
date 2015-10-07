// yakc.c

#include yaku.h
#include yakk.h
#include clib.h

#define TaskStackSize 256

#define IDLETASK 0
#define ATASK 1
#define BTASK 2
#define CTASK 3
#define DTASK 4

int idleTaskStack[TaskStackSize];
int TCBArrayNum = 1;
bool runSched = false;

void YKInitialize(){// - Initializes all required kernel data structures
    //initialize task one data
    YKTCBArray[IDLETASK].priority = INT_MAX;
    YKTCBArray[IDLETASK].SPtr = idleTaskStack[TaskStackSize-2];//256 + 1 - 3 (for iret)
    YKTCBArray[IDLETASK].state = ready_st; //will be running
    YKTCBArray[IDLETASK].tickDelay = 0;
//    currentTask = &YKTCBArray[IDLETASK];
    readyTask = &YKTCBArray[IDLETASK];
    YKReadyList = &YKTCBArray[IDLETASK];
//    YKTCBArray[IDLETASK].prevTCB = NULL;
//    YKTCBArray[IDLETASK].nextTCB = NULL;
    //initialize task two data
    //initialize task three data
    //initialize task four data
}

void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority ){//Creates a new task
	 
    YKTCBArray[TCBArrayNum].priority = priority;
    YKTCBArray[TCBArrayNum].SPtr = taskStack + 254; //256 + 1 - 3 (for iret)
    YKTCBArray[TCBArrayNum].state = ready_st; //will be running
    YKTCBArray[TCBArrayNum].tickDelay = 0;
    YKTCBArray[TCBArrayNum].nextInst = task;
    //if the top of ready Task list is less priority need to switch the priority
    adjustPriority(YKReadyList,&YKTCBArray[TCBArrayNum], &YKReadyList);
    if (readyTask != YKReadyList) {
   	 runSched = true;
    }
    //need to set up the IP, Cs and flags
    *YKTCBArray[TCBArrayNum].SPtr = task;
    *(YKTCBArray[TCBArrayNum].SPtr+2)=0;//plus 2 because *void are size of char
    *(YKTCBArray[TCBArrayNum].SPtr+4)=0;
    TCBArrayNum++;
}

void adjustPriority(TCBptr listNode, TCBptr toAdd, TCBptr *topOfList){
    static int priorityDepth = 0;
    if (listNode == NULL || toADD == NULL)
   	 return;
    
    if (listNode->priority > toAdd->priority) {
   	 toAdd->prevTCB = listNode->prevTCB;
   	 toAdd->nextTCB = listNode;
   	 listNode->prevTCB->nextTCB = toAdd;
   	 listNode->prevTCb = toAdd;
   	 //listNode = toAdd;
   	 if (priorityDepth == 0)
   		 *topOfList = toAdd;
    } else {
   	 priorityDepth++;
   	 adjustPriority(listNode->nextTCB, toAdd, topOfList);
   	 priorityDepth--;
    }
}
void YKRun(){// Starts actual execution of user code

}

void YKDelayTask(unsigned int){// Delays a task for specified number of clock ticks

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
    if (runSched) {    
   	 runSched = false;
   	 YKDispatcher();
    }    
}

void YKDispatcher(){// Begins or resumes execution of the next task

}

void YKTickHandler(){// The kernel's timer tick interrupt handler
}

void YKCtxSwCount(){// Global variable tracking context switches
}

void YKIdleCount(){// Global variable used by idle task
}

void YKTickNum(){// Global variable incremented by tick handler
}

void printLinkedList(TCBptr head, char* string){
  //string is user input to know what list is printing
  TCBptr current = head;
  printNewLine();
  printString("##############\n");
  printString("contents of ");
  printString(string);
  printNewLine();
  while (current != NULL) {
	printString("Priority = ");
	printInt(head->priority);
	printString(" TickDelay = ");
	printInt(head->tickDelay);
	printString(" State = ");
	printInt(head->state);
	printNewLine();
	current = current->nextTCB;
  }
  printString("##############\n");
}

