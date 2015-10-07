void YKInitialize();// - Initializes all required kernel data structures

void YKNewTask(void(*task)(void), void *taskStack, unsigned char priority );//Creates a new task

void YKRun();// Starts actual execution of user code

void YKDelayTask(unsigned int delay);// Delays a task for specified number of clock ticks

void YKEnterMutex();// Disables interrupts

void YKExitMutex();// Enables interrupts

