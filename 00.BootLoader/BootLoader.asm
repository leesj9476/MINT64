[ORG 0X00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT:	dw 0x02

;;;;;;;;;;;;;;;;
; Code Section ;
;;;;;;;;;;;;;;;;
START:
	mov ax, 0x07C0
	mov ds, ax
	mov ax, 0xB800
	mov es, ax

	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFE
	mov bp, 0xFFFE

	mov si, 0

.SCREENCLEARLOOP:
	mov byte [es : si], 0
	mov byte [es : si + 1], 0x0A

	add si, 0x02
	cmp si, 4000 ; 80 chars * 25 lines * 2 bytes
	jl .SCREENCLEARLOOP

	push BOOTLOADERSTARTMESSAGE
	push 0
	push 0
	call PRINTMESSAGE
	add sp, 6

	push IMAGELOADINGMESSAGE
	push 1
	push 0
	call PRINTMESSAGE
	add sp, 6

RESETDISK:
	mov ah, 0
	mov dl, 0
	int 0x13
	jc  HANDLEDISKERROR

	mov si, 0x1000
	mov es, si
	mov bx, 0x0000

	mov di, word [TOTALSECTORCOUNT]

READDATA:
	cmp di, 0
	je READEND
	sub di, 1

	mov ah, 0x02
	mov al, 0x01
	mov ch, byte [TRACKNUMBER]
	mov cl, byte [SECTORNUMBER]
	mov dh, byte [HEADNUMBER]
	mov dl, 0x00
	int 0x13
	jc HANDLEDISKERROR

	add si, 0x0020
	mov es, si

	mov al, byte [SECTORNUMBER]
	add al, 0x01
	mov byte [SECTORNUMBER], al
	cmp byte [SECTORNUMBER], 19
	jl READDATA

	xor byte [HEADNUMBER], 0x01
	mov byte [SECTORNUMBER], 0x01

	cmp byte [HEADNUMBER], 0x00
	jne READDATA

	add byte [TRACKNUMBER], 0x01
	jmp READDATA

READEND:
	push LOADINGCOMPLETEMESSAGE
	push 1
	push 20
	call PRINTMESSAGE
	add sp, 6

	jmp 0x1000:0x0000

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Print Message Function ;
; Args: (X, Y, Str)      ;
;;;;;;;;;;;;;;;;;;;;;;;;;;
PRINTMESSAGE:
	push bp
	mov bp, sp

	push es
	push ax
	push cx
	push si
	push di

	mov ax, 0xB800
	mov es, ax

	; Calculate Y
	mov ax, word [bp + 6]
	mov si, 160
	mul si
	mov di, ax

	; Calculate X
	mov ax, word [bp + 4]
	mov si, 2
	mul si
	add di, ax

	; si <- Str address
	mov si, word [bp + 8]

.MESSAGELOOP:
	mov cl, byte [si]
	cmp cl, 0
	je .MESSAGEEND

	mov byte [es: di], cl
	add si, 1
	add di, 2

	jmp .MESSAGELOOP

.MESSAGEEND:
	pop di
	pop si
	pop cx
	pop ax
	pop es
	pop bp

	ret

HANDLEDISKERROR:
	push DISKERRORMESSAGE
	push 4
	push 0
	call PRINTMESSAGE
	add sp, 6

	jmp $

;;;;;;;;;;;;;;;;
; Data Section ;
;;;;;;;;;;;;;;;;
BOOTLOADERSTARTMESSAGE:	db 'MINT64 OS Boot Loader Start~!!', 0
DISKERRORMESSAGE:		db 'DISK Error~!!', 0
IMAGELOADINGMESSAGE:	db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete~!!', 0

SECTORNUMBER:	db 0x02
HEADNUMBER:		db 0x00
TRACKNUMBER:	db 0x00

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Fill the rest space of boot loader with 0 ;
; The last two bytes are 0x55, 0xAA         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
times 510 - ($ - $$)	db 0x00
db 0x55
db 0xAA
