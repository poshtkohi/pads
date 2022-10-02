; includelib user32.lib
; includelib kernel32.lib
; includelib shell32.lib

; extrn MessageBoxA: PROC
; extrn ExitProcess: PROC
PUBLIC Parvicursor_AtomicCompareAndSwap_x64

; .data
; mytit db 'The 64-bit world of Windows & assembler...', 0
; mymsg db 'Hello World!', 0


.code
Parvicursor_AtomicCompareAndSwap_x64 PROC uses rdx
	;mov rax,[rcx]
	;mov rbx,[rdx]
	;mov [rcx],rbx
	;mov [rdx],rax
	;ret
	
	mov rax,[rcx] ; Load the value of the accum variable into rax. The rcx resgiter contains the address of the accum.
	; mov rbx,rdx ; dest
	; mov rdi,r8 ;newval
    lock cmpxchg [rdx],r8 ; The r8 register conatins the value of newval and rdx contains the address of the dest. 
	jne Unsuccessful ;Jump if ZF=0
	mov rax,1 ; success = true;
	ret

Unsuccessful:
	mov [rcx],rax ; Copy the rax content into the location pointed by accum.
	mov rax,0 ; success = false;
	ret

Parvicursor_AtomicCompareAndSwap_x64 ENDP

End