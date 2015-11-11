// Header file for clib.s output routines
#ifndef YAKK_H
#define YAKK_H

#define TASKNUMBER 6
#define MAX_SEMAPHORE 5
#define MAX_QUEUE 5
#define c 5
#define true 1
#define false 0
#define NULL 0

#define EVENT_WAIT_ANY 0
#define EVENT_WAIT_ALL 1

typedef char byte;
typedef char bool;
typedef unsigned int uint;

enum task_st {ready_st, delayed_st};

typedef struct taskblock* TCBptr;// struct pointer for TCB's and lists
typedef struct taskblock {// TCB data structure
	void * SPtr;
	enum task_st state;
	int priority;
	int tickDelay;
	TCBptr prevTCB;
	TCBptr nextTCB;
	uint eventMask; 
	int waitMode;	
} TCB;

typedef struct semaphore {
	int value;
	TCBptr pendingList;
} YKSEM;

typedef struct YKQ_t {
	void** baseAddr;
	uint size;
	int head;
	int tail;
	uint elCount;
	TCBptr pendingList;
} YKQ;

typedef struct YKEVENT_t {
	uint mask;
	TCBptr pendingList;

} YKEVENT;


void idleTask(void);

extern TCBptr currentTask;
extern TCBptr YKReadyList;//linked list of ready tasks
extern TCBptr YKSuspList;//linked list of Suspended tasks
extern TCBptr YKDelayList;//linked list of delayed tasks
extern TCBptr YKAvailList;//list of available tasks 
extern TCB YKTCBArray[TASKNUMBER+1];
extern unsigned int YKIdleCount;
extern unsigned YKTickNum;

//semaphore
YKSEM* YKSemCreate(int init);
void YKSemPend(YKSEM* sem);
void YKSemPost(YKSEM* sem);
extern YKSEM YKSEMArray[MAX_SEMAPHORE];
void printSemList(char* string, YKSEM* sem);

//queues
YKQ* YKQCreate(void** start, uint size);
void* YKQPend(YKQ* queue);
int YKQPost(YKQ* queue, void* msg);
extern YKQ YKQArray[MAX_QUEUE];

//events
YKEVENT* YKEventCreate(uint initialValue);
uint YKEventPend(YKEVENT* event, uint eventMask, int waitMode);
void YKEventSet(YKEVENT* event, uint eventMask);
void YKEventReset(YKEVENT* event, uint eventMask);
extern YKEVENT YKEVENTArray[MAX_EVENT];


#endif // YAKK_H

