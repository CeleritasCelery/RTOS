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
void YKCtxSwCount();
void YKIdleCount();
void YKTickNum();

void printLinkedList(char* string);

#endif
