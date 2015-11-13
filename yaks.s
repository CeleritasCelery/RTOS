YKDispatcher:
	pop     ax
	pushf
	push    cs
	push    ax
	push	bp
	mov	bp, sp
	push	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	es
	push 	ds
	mov	si, word [currentTask]
	mov	word [si], sp
	mov	si, word [YKReadyList]
	mov	word [currentTask], si ;change the context to new task
	mov	si, word [currentTask] 
	mov	sp, word [si]
	mov 	al, 0x20
	out	0x20, al ; write to pic
	pop 	ds
	pop	es
	pop 	di
	pop 	si
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
	pop	bp
	iret

	
