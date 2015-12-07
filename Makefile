#####################################################################
# ECEn 425 Lab 7 Makefile

simptris.bin:	simptrisfinal.s
		@nasm simptrisfinal.s -o simptris.bin -l simptris.lst

simptrisfinal.s:	clib.s simptris.s simptrisIsr.s simptrisInth.s simptrisApp.s yakc.s yaks.s
		@cat clib.s simptris.s simptrisIsr.s simptrisInth.s simptrisApp.s yakc.s yaks.s > simptrisfinal.s

simptrisApp.s:	simptrisApp.c
		@cpp -xc++ simptrisApp.c simptrisApp.i
		@c86 -g simptrisApp.i simptrisApp.s

simptrisInth.s:	simptrisInth.c
		@cpp -xc++ simptrisInth.c simptrisInth.i
		@c86 -g simptrisInth.i simptrisInth.s

yakc.s:	        yakc.c
		@cpp -xc++ yakc.c yakc.i
		@c86 -g yakc.i yakc.s

clean:
		@if [ -e simptris.bin    ] ; then rm simptris.bin    ; fi
		@if [ -e simptris.lst    ] ; then rm simptris.lst    ; fi
		@if [ -e simptrisfinal.s ] ; then rm simptrisfinal.s ; fi
		@if [ -e simptrisInth.s  ] ; then rm simptrisInth.s  ; fi
		@if [ -e simptrisInth.i  ] ; then rm simptrisInth.i  ; fi
		@if [ -e simptrisApp.s   ] ; then rm simptrisApp.s   ; fi
		@if [ -e simptrisApp.i   ] ; then rm simptrisApp.i   ; fi
		@if [ -e yakc.s          ] ; then rm yakc.s      ; fi
		@if [ -e yakc.i          ] ; then rm yakc.i      ; fi
