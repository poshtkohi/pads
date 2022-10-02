.386
.model flat, c

PUBLIC SimpleISR_x86

.code
SimpleISR_x86 PROC; FAR

	mov eax, -10
	;iret
	ret

SimpleISR_x86 ENDP

End