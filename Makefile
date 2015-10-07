#####################################################################
# ECEn 425 Lab 4 Makefile

lab4.bin:	lab4final.s
		nasm lab4final.s -o lab4.bin -l lab4.lst

lab4final.s:	clib.s myisr.s myinth.s lab4app.s yakc.s yaks.s
		cat clib.s myisr.s myinth.s lab4app.s yakc.s yaks.s > lab4final.s

lab4app.s:	lab4app.c
		cpp lab4app.c lab4app.i
		c86 -g lab4app.i lab4app.s

myinth.s:	myinth.c
		cpp myinth.c myinth.i
		c86 -g myinth.i myinth.s

yakc.s:	        yakc.c
		cpp yakc.c yakc.i
		c86 -g yakc.i yakc.s

clean:
		rm lab4.bin lab4.lst lab4final.s myinth.s myinth.i \
		lab4app.s lab4app.i yakc.s yakc.i
