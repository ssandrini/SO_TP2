
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _int80Handler
GLOBAL _getKey
GLOBAL _keyPressed
GLOBAL _RTC
GLOBAL _cpuid
GLOBAL _cpuidSupport
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN sysHandler
EXTERN getStackBase
EXTERN scheduler
EXTERN timer_handler

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1    ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro exceptionHandler 1
	pushState
	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp   ; le paso el rsp para que tenga el stackframe
	call exceptionDispatcher
	popState
	iretq 
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	pushState
	call timer_handler
	popState
	pushState
	mov rdi, rsp
	call scheduler
	mov rsp, rax	; agarro el rsp del prox proceso
	popState
	mov al, 20		; EOI para el pic
	out 20h, al
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Instruction Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

_keyPressed:
	in al,64h
	and al,0x01 
	ret

_getKey:
	in al,60h
	ret

_int80Handler:
	pushState
	
	mov r8, rsp
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax
	call sysHandler
	
	popState
	iretq

_RTC:
	push rbp
	mov rbp, rsp
				
	mov rax, rdi
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

; devuelve 1 si el micro soporta la instruccion cpuid y 0 sino
_cpuidSupport:
	push rbp
	mov rbp, rsp
	push rbx  			; pusheo rbx porque desp lo voy a usar
	pushfq    			; pusheo el registro de flags
	pop rax   			; lo meto en rax
	mov rbx, rax		; lo guardo en rbx

	xor rax, 200000h 	; seteo el ID flag en 1
	push rax		    ; pusheo este registro
	popfq		 		; popeo los flags
	pushfq		 		; pusheo los flags
	pop rax		 		; los popeo en rax
	cmp rax, rbx	 	; los comparo con los flags viejos

	jz .no_support	
	pop rbx
	mov rax, 1
	mov rsp, rbp
	pop rbp
	ret

.no_support:
	pop rbx
	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret

;int _cpuid(uint32_t * r1,uint32_t * r2, int * id);
; 				rdi 		rsi			  rdx
_cpuid:
	push rcx
	mov rax, [rdx]
	mov rcx, 0
	cpuid
	cmp dword[rdx], 7h
	je .special
	mov dword[rdi], ecx
	mov dword[rsi], edx
	pop rcx
	ret
.special:
	mov dword[rdi], ecx
	mov dword[rsi], ebx
	pop rcx
	ret


SECTION .bss
	aux resq 1