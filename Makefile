#####################################################################
# ECEn 425 Lab 4 Makefile

lab6.bin:	lab6final.s
		nasm lab6final.s -o lab6.bin -l lab6.lst

lab6final.s:	clib.s myisr.s myinth.s lab6app.s yakc.s yaks.s
		cat clib.s myisr.s myinth.s lab6app.s yakc.s yaks.s > lab6final.s

lab6app.s:	lab6app.c
		cpp -xc++ lab6app.c lab6app.i
		c86 -g lab6app.i lab6app.s

myinth.s:	myinth.c
		cpp -xc++ myinth.c myinth.i
		c86 -g myinth.i myinth.s

yakc.s:	        yakc.c
		cpp -xc++ yakc.c yakc.i
		c86 -g yakc.i yakc.s

clean:
		rm lab6.bin lab6.lst lab6final.s myinth.s myinth.i \
		lab6app.s lab6app.i yakc.s yakc.i
