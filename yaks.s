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

YKSemPost: 
	push	bp
	mov	bp, sp
	ALIGN	2
	cli
	mov	si, word [bp+4]
	inc	word [si]
	mov	ax, word [si+2]
	test	ax, ax
	je	no_pend
	mov	si, word [bp+4]
	add	si, 2
	mov	ax, word [si]
	mov	word [bp-2], ax
	push	word [bp-2]
	mov	ax, word [bp+4]
	add	ax, 2
	push	ax
	call	deleteFromLinkedList
	add	sp, 4
	push	word [bp-2]
	mov	ax, YKReadyList
	push	ax
	call	addToLinkedList
	add	sp, 4
	mov	ax, word [nestedDepth]
	test	ax, ax
	jne	nested_depth_zero
	call	YKScheduler
nested_depth_zero:
no_pend:
	sti
	mov	sp, bp
	pop	bp
	ret

YKSemPend:

	push	bp
	mov	bp, sp
	ALIGN	2
	cli
	mov	si, word [bp+4]
	mov	ax, word [si]
	dec	word [si]
	test	ax, ax
	jg	sem_availible
	push	word [currentTask]
	mov	ax, YKReadyList
	push	ax
	call	deleteFromLinkedList
	add	sp, 4
	push	word [currentTask]
	mov	ax, word [bp+4]
	add	ax, 2
	push	ax
	call	addToLinkedList
	add	sp, 4
	call	YKScheduler
sem_availible:
	sti
	mov	sp, bp
	pop	bp
	ret




