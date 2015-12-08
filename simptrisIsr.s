
IsrTick:
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	bp
	push 	es
	push 	ds  
	call 	YKEnterISR
	sti	
	call    tickHandler
	cli
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop 	ds
	pop 	es
	pop 	bp
	pop 	di
	pop 	si
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
	iret

IsrGameOver:
	push 	ax
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	call    resetHandler
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrNewPiece:
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	bp
	push 	es
	push 	ds 
	call 	YKEnterISR 
	sti	
	call    newPieceHandler
	cli
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop 	ds
	pop 	es
	pop 	bp
	pop 	di
	pop 	si
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
	iret

IsrReceived:
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	bp
	push 	es
	push 	ds 
	call	YKEnterISR 
	sti	
	call    recievedHandler
	cli
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop 	ds
	pop 	es
	pop 	bp
	pop 	di
	pop 	si
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
	iret

IsrTouchdown:
	push 	ax
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrClear:
	push 	ax
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrKey:
	push 	ax
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrReset:
	sti	
	call    resetHandler
	cli
