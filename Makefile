#####################################################################
# ECEn 425 Lab 7 Makefile

lab7.bin:	lab7final.s
		@nasm lab7final.s -o lab7.bin -l lab7.lst

lab7final.s:	clib.s myisr.s lab7inth.s lab7app.s yakc.s yaks.s
		@cat clib.s myisr.s lab7inth.s lab7app.s yakc.s yaks.s > lab7final.s

lab7app.s:	lab7app.c
		@cpp -xc++ lab7app.c lab7app.i
		@c86 -g lab7app.i lab7app.s

lab7inth.s:	lab7inth.c
		@cpp -xc++ lab7inth.c lab7inth.i
		@c86 -g lab7inth.i lab7inth.s

yakc.s:	        yakc.c
		@cpp -xc++ yakc.c yakc.i
		@c86 -g yakc.i yakc.s

clean:
		@if [ -e lab7.bin    ] ; then rm lab7.bin    ; fi
		@if [ -e lab7.lst    ] ; then rm lab7.lst    ; fi
		@if [ -e lab7final.s ] ; then rm lab7final.s ; fi
		@if [ -e lab7inth.s  ] ; then rm lab7inth.s  ; fi
		@if [ -e lab7inth.i  ] ; then rm lab7inth.i  ; fi
		@if [ -e lab7app.s   ] ; then rm lab7app.s   ; fi
		@if [ -e lab7app.i   ] ; then rm lab7app.i   ; fi
		@if [ -e yakc.s      ] ; then rm yakc.s      ; fi
		@if [ -e yakc.i      ] ; then rm yakc.i      ; fi
