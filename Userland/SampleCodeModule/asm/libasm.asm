GLOBAL _quadratic
GLOBAL _exc6Trigger
GLOBAL _getCpuInfo
GLOBAL _FtoCelcius
GLOBAL _CtoFahren
GLOBAL _rectToPolar
GLOBAL _syscall
section .text

_syscall:
    int 80h
    ret

; https://www.felixcloutier.com/x86/ud
_exc6Trigger:
	UD2
	ret

; int _quadratic(long double * a, long double * b, long double * c, long double * r1, long double * r2)
_quadratic:
    push rbp              ; armado del stackframe
    mov rbp, rsp
   
    fninit                
    fld   tword[rdi]      ; st0 = a
    fadd    st0           ; st0 = 2a
    fld   tword[rsi]      ; st0 = b   ; st1 = 2a
    fmul    st0           ; st0 = b²  ; st1 = 2a
    fld   tword[rdx]      ; st0 = c   ; st1 = b²  ;  st2 = 2a
    fmul    st2           ; st0 = 2ac ; st1 = b²  ;  st2 = 2a
    fadd    st0           ; st0 = 4ac ; st1 = b²  ;  st2 = 2a
    fsubp                 ; st0 = b²-4ac ; st1 = 2a
    ; tengo que chequear si st0 es menor que 0
    fldz                  ; st0 = 0.0 ; st1 = b²-4ac ; st2 = 2a

    ; al finalizar la comparacion me deja los flags en EFLAGS
    fcomip st0, st1       ; st0 = b²-4ac ; st1 = 2a
    ja .no_results        ; si es negativo
    ; si sigo aca es porque el discriminante era positivo
    fsqrt                 ; st0 = raiz de b²-4ac ; st1 = 2a
    
    ; calculo de la primera raiz
    fld    tword[rsi]     ; st0 = b   ; st1 = raiz de b²-4ac ; st2 = 2a
    fchs                  ; st0 = -b   ; st1 = raiz de b²-4ac ; st2 = 2a
    fsub    st1           ; st0 = -b - raiz de b²-4ac  ; st1 = raiz de b²-4ac ; st2 = 2a
    fdiv    st2           ; st0 = (-b - raiz de b²-4ac) / 2a  ; st1 = raiz de b²-4ac ; st2 = 2a
                          
    fstp    tword[rcx]    ; st0 = raiz de b²-4ac ; st1= 2a
    ;calculo de la segunda raiz
    fld    tword[rsi]     ; st0 = b   ; st1 = raiz de b²-4ac ; st2 = 2a
    fchs                  ; st0 = -b   ; st1 = raiz de b²-4ac ; st2 = 2a
    fadd    st1           ; st0 = -b + raiz de b²-4ac  ; st1 = raiz de b²-4ac ; st2 = 2a
    fdiv    st2           ; st0 = (-b + raiz de b²-4ac) / 2a  ; st1 = raiz de b²-4ac ; st2 = 2a
    fstp    tword[r8]     ; st0 = raiz de b²-4ac ; st1= 2a

    mov rax, 1            ; devulevo 1 porque pude calcular
    mov rsp, rbp
    pop rbp
    ret

.no_results:
    mov rax, 0            ; devulevo 0 porque no pude calcular
    mov rsp, rbp
    pop rbp
    ret


_FtoCelcius:                   ;rdi                ; rsi                ;rdx              rcx
; extern void FtoC_(long double * deg_f, long double * c1, long double * c2 , long double * res);
    push rbp              ; armado del stackframe
    mov rbp, rsp
    fninit
    fld tword [rsi]          ; st0 = 5/9
    fld tword [rdi]          ; st0 = deg    ; st1 = 5/9
    fld tword [rdx]          ; st0 = 32     ; st1 =  deg    ; st2 = 5/9   
    fsubp 
    fmulp 
    fstp  tword[rcx]
    mov rsp, rbp
    pop rbp
    ret

_CtoFahren:
; extern void _CtoFahren(long double * deg_c, long double * c1, long double * c2 , long double * res);
    push rbp              ; armado del stackframe
    mov rbp, rsp
    fninit
    fld tword [rsi]          
    fld tword [rdi]          
    fmulp                    
    fld tword [rdx]          
    faddp 
    fstp  tword[rcx]
    mov rsp, rbp
    pop rbp
    ret

_rectToPolar:
    ;                            rdi                rsi             rdx                 rcx
    ;extern void _rectToPolar(long double * x, long double * y, long double * mod, long double * angle, long double * c);
    push rbp                 ; armado del stackframe
    mov rbp, rsp
    fninit
    fld tword [rsi]          ; st0 = y
    fld tword [rdi]          ; st0 = x    ; st1 = y
    fpatan                   ; st0 = atan(y/x)
    fld tword [r8]           
    fldpi                    
    fdiv
    fmul                     ; st0 = angle
    fstp tword[rcx]
    fld  tword[rdi]
    fmul st0, st0
    fld  tword[rsi]
    fmul st0, st0
    faddp
    fsqrt
    fstp tword[rdx]
    mov rsp, rbp
    pop rbp
    ret
