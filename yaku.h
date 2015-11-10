#ifndef YAKU_H
#define YAKU_H

void YKInitialize();
void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority );
void YKRun();
void YKDelayTask(unsigned int);
void YKEnterMutex();
void YKExitMutex();
void YKEnterISR();
void YKExitISR();
void YKScheduler();
void YKDispatcher();
void YKTickHandler();
int getYKCtxSwCount();

void printIntHex(int arg);
void printVarHex(char* name, int var);
void printVar(char* name, int var);
void printATask(void);
void printLinkedList(char* string, int list);

extern YKCtxSwCount;

#endif

