;***********************************************************************************************************************
;
;		ReaverOS
;		floppy.asm
;		floppy routines
;
;***********************************************************************************************************************

bits	16

sector			db 0
head			db 0
track			db 0

sectorsize:		dw 512 			; size of single sector								13, 14
nsectors:		dw 2880			; number of sectors									15, 16
tracksectors:	dw 18			; number of sectors per track						17, 18
cylinderheads:	dw 2			; number of heads per cylinder						19, 20

;***********************************************************************************************************************
;
;	absolute_address()
;	ax - address to be converted
;
;***********************************************************************************************************************
absolute_address:
	xor		dx, dx
	div		word [tracksectors]
	inc		dl
	mov		byte [sector], dl
	xor		dx, dx
	div		word [cylinderheads]
	mov		byte [head], dl
	mov		byte [track], al
	ret

;***********************************************************************************************************************
;
;	read_sectors()
;	cx - number of sectors
;	ax - starting sector
;	es:bx - buffer to read to
;
;***********************************************************************************************************************
read_sectors:
	push	cx
	.begin:
		mov		di, 0x005
	.loop:
		push	ax
		push	bx
		push	cx
		call	absolute_address		; convert address
		mov		ah, 0x02				; read sector
		mov		al, 0x01				; read ONE sector
		mov		ch, byte [track]
		mov		cl, byte [sector]
		mov		dh, byte [head]
		mov		dl, byte [bootdrive]
		int		0x13
		jnc		.end					; test read error
		xor		ax, ax
		int		0x13					; reset disc
		dec		di
		pop		cx
		pop		bx
		pop		ax
		jnz		.loop

		mov		si, fail
		call	print16
		int		0x16
		int		0x19
	.end:
		mov		si, progress
		call	print16
		
		pop		cx
		pop		bx
		pop		ax
		add		bx, word [sectorsize]
		inc		ax
		loop	.begin
		pop		cx
		ret
