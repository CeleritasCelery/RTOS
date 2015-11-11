# 1 "lab7inth.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "lab7inth.c"






# 1 "lab6defs.h" 1
# 9 "lab6defs.h"
struct msg
{
    int tick;
    int data;
};
# 8 "lab7inth.c" 2
# 1 "yakk.h" 1
# 16 "yakk.h"
typedef char byte;
typedef char bool;
typedef unsigned int uint;

enum task_st {ready_st, delayed_st};

typedef struct taskblock* TCBptr;
typedef struct taskblock {
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
extern TCBptr YKReadyList;
extern TCBptr YKSuspList;
extern TCBptr YKDelayList;
extern TCBptr YKAvailList;
extern TCB YKTCBArray[6 +1];
extern unsigned int YKIdleCount;
extern unsigned YKTickNum;


YKSEM* YKSemCreate(int init);
void YKSemPend(YKSEM* sem);
void YKSemPost(YKSEM* sem);
extern YKSEM YKSEMArray[5];
void printSemList(char* string, YKSEM* sem);


YKQ* YKQCreate(void** start, uint size);
void* YKQPend(YKQ* queue);
int YKQPost(YKQ* queue, void* msg);
extern YKQ YKQArray[5];


YKEVENT* YKEventCreate(uint initialValue);
uint YKEventPend(YKEVENT* event, uint eventMask, int waitMode);
void YKEventSet(YKEVENT* event, uint eventMask);
void YKEventReset(YKEVENT* event, uint eventMask);
extern YKEVENT YKEVENTArray[5];
# 9 "lab7inth.c" 2
# 1 "clib.h" 1



void print(char *string, int length);
void printNewLine(void);
void printChar(char c);
void printString(char *string);


void printInt(int val);
void printLong(long val);
void printUInt(unsigned val);
void printULong(unsigned long val);


void printByte(char val);
void printWord(int val);
void printDWord(long val);


void exit(unsigned char code);


void signalEOI(void);
# 10 "lab7inth.c" 2
# 1 "lab7defs.h" 1
# 15 "lab7defs.h"
extern YKEVENT *charEvent;
extern YKEVENT *numEvent;
# 11 "lab7inth.c" 2

extern YKQ *MsgQPtr;
extern struct msg MsgArray[];
extern int GlobalFlag;
extern int KeyBuffer;

void myreset(void)
{
    exit(0);
}

void mytick(void)
{

}

void KeyboardHandler(void)
{
    char c;
    c = KeyBuffer;

    if(c == 'a') YKEventSet(charEvent, 0x1);
    else if(c == 'b') YKEventSet(charEvent, 0x2);
    else if(c == 'c') YKEventSet(charEvent, 0x4);
    else if(c == 'd') YKEventSet(charEvent, 0x1 | 0x2 | 0x4);
    else if(c == '1') YKEventSet(numEvent, 0x1);
    else if(c == '2') YKEventSet(numEvent, 0x2);
    else if(c == '3') YKEventSet(numEvent, 0x4);
    else {
        print("\nKEYPRESS (", 11);
        printChar(c);
        print(") IGNORED\n", 10);
    }
}
