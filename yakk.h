// yakk.h

int idleCount = 0;
int contextSwitchNum = 0;

// Header file for clib.s output routines
#define TASKNUMBER 4
#define true 1
#define false 0
#define NULL 0
typedef char bool;

typedef struct task_block* TCBptr;//struct pointer for TCB's and lists

typedef enum
{
	ready_st,
	delayed_st
} task_st;

typedef struct task_block{//TCB data structure
  void * SPtr;
  void * nextInst;
  int priority;
  int tickDelay;
  task_st state;
  TCBptr prevTCB;
  TCBptr nextTCB;
} TCB;

//TCBptr currentTask;
TCBptr readyTask;
TCBptr YKReadyList;//linked list of ready tasks
TCBptr YKSuspList;//linked list of Suspended tasks
TCBptr YKAvailList;//list of available tasks

TCB YKTCBArray[TASKNUMBER+1];
int nestedDepth = 0;

void YKTickHandler();// The kernel's timer tick interrupt handler

void YKCtxSwCount();// Global variable tracking context switches

void YKIdleCount();// Global variable used by idle task

void YKTickNum();// Global variable incremented by tick handler

void YKEnterISR();// Called on entry to ISR

void YKExitISR();// Called on exit from ISR

void YKScheduler();// Determines the highest priority ready task

void YKDispatcher();// Begins or resumes execution of the next task

void YKTickHandler();// The kernel's timer tick interrupt handler

void YKCtxSwCount();// Global variable tracking context switches

void YKIdleCount();// Global variable used by idle task

void YKTickNum();// Global variable incremented by tick handler

