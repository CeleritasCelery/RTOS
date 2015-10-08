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
int getYKIdleCount();
void YKTickNum();

void printLinkedList(char* string, int list);

extern YKCtxSwCount;

#endif

