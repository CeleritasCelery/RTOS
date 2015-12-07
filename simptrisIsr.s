
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
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
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
	sti	
	call    newPieceHandler
	cli
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
	sti	
	call    recievedHandler
	cli
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
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrClear:
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	bp
	push 	es
	push 	ds  
	sti	
	call    clearHandler
	cli
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

IsrKey:
	push 	ax
	call	YKExitISR
	mov	al, 0x20	; Load nonspecific EOI value (0x20) into register al
	out	0x20, al	; Write EOI to PIC (port 0x20)
	pop	ax
	iret

IsrReset:
	sti	
	call    resetHandler
	cli
