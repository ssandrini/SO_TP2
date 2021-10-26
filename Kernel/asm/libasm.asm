GLOBAL cpuVendor
GLOBAL _getRSP
GLOBAL _xchg
GLOBAL _int20
section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

_getRSP:
	mov rax, rsp
	ret

_xchg:
    mov rax, rsi
    xchg [rdi], eax
    ret

_int20:
	int 20h
	ret