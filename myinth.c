
#include "clib.h"
#include "yakk.h"

extern int KeyBuffer;

void interruptHandler_Reset() {
	exit(0);
}


void interruptHandler_Keyboard() {
	char key = (char)KeyBuffer;
	int inc = 0;
	if (key < ' ' || key > '~') return;

	if (key == 'd') {
		printNewLine();
		printString("DELAY KEY PRESSED");
		printNewLine();	
		while (inc++ < 5000);
		printNewLine();
		printString("DELAY COMPLETE");
		printNewLine();
	} else if (key == 'p') {
		YKSemPost(&YKSEMArray[3]);
	} else {
		printNewLine();
		printString("KEYPRESS (");
		printChar(key);
		printString(") IGNORED");
		printNewLine();
	}
}
