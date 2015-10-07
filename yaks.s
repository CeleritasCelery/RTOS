saveContext:
	push	ax
	push 	bx
	push 	cx
	push 	dx
	push 	si
	push 	di
	push 	bp
	push 	es
	push 	ds
	ret

restoreContext:
	pop ds
	pop es
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	ret

YKDispatcher:
	
	ret
