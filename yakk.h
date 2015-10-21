// Header file for clib.s output routines
#ifndef YAKK_H
#define YAKK_H

#define TASKNUMBER 4
#define true 1
#define false 0
#define NULL 0

typedef char bool;
typedef struct taskBlock *TCBptr;//struct pointer for TCB's and lists
enum task_st {ready_st, delayed_st};
typedef struct taskBlock{//TCB data structure
  void * SPtr;
//  void * nextInst;
  enum task_st state;
  int priority;
  int tickDelay;
//  TCBptr prevTCB = NULL;
//  TCBptr nextTCB = NULL;
  TCBptr prevTCB;
  TCBptr nextTCB;
} TCB;

void idleTask(void);

extern TCBptr currentTask;
extern TCBptr YKReadyList;//linked list of ready tasks
extern TCBptr YKSuspList;//linked list of Suspended tasks
extern TCBptr YKDelayList;//linked list of delayed tasks
extern TCBptr YKAvailList;//list of available tasks 
extern struct taskBlock YKTCBArray[TASKNUMBER+1];
extern int YKIdleCount;


#endif

