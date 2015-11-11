#####################################################################
# ECEn 425 Lab 4 Makefile

lab7.bin:	lab7final.s
		nasm lab7final.s -o lab7.bin -l lab7.lst

lab7final.s:	clib.s myisr.s lab7inth.s lab7app.s yakc.s yaks.s
		cat clib.s myisr.s lab7inth.s lab7app.s yakc.s yaks.s > lab7final.s

lab7app.s:	lab7app.c
		cpp -xc++ lab7app.c lab7app.i
		c86 -g lab7app.i lab7app.s

lab7inth.s:	lab7inth.c
		cpp -xc++ lab7inth.c lab7inth.i
		c86 -g lab7inth.i lab7inth.s

yakc.s:	        yakc.c
		cpp -xc++ yakc.c yakc.i
		c86 -g yakc.i yakc.s

clean:
		rm lab7.bin lab7.lst lab7final.s lab7inth.s lab7inth.i \
		lab7app.s lab7app.i yakc.s yakc.i
