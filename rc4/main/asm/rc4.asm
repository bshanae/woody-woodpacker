global					rc4


section					.bss
s:						resb	256


section					.text
						; rc4 - encrypt given text using RC4
						; rdi ~ key
						; rsi ~ key length
						; rdx ~ plaintext
						; rcx ~ ciphertext
						; r8 ~ texts' length
rc4:					push	rdi
						push	rsi
						push	rdx
						push	rcx
						push	r8

						call	initialize_s

						pop		r8
						pop		rcx
						pop		rdx
						pop		rsi
						pop		rdi

						mov		rdi, rdx
						mov		rsi, rcx
						mov		rdx, r8
						call	encrypt_with_keystream

						ret

						; initialize_s - initialize array S, that later will be used for generating keystream
						; rdi ~ key
						; rsi ~ key length
initialize_s:			nop
set_s:					mov		rcx, 0
set_s_iteration:		mov		byte [s + rcx], cl
						inc		rcx
						cmp		rcx, 256
						jl		set_s_iteration

permut_s:				mov		r8, 0					; r8 ~ i
						mov		r9, 0					; r9 ~ j 
permut_s_iteration:		mov		rax, r8					; rdx = i % key length
						xor		rdx, rdx
						mov		rbx, rsi
						div		rbx
						add		r9b, byte [s + r8]		; j += s[i]
						add		r9b, byte [rdi + rdx]	; j += key[i % key length]
						mov		rax, r9						; j = j % 256
						xor		rdx, rdx
						mov		rbx, 256
						div		rbx
						mov		r9, rdx
						mov		r10b, byte [s + r8]		; swap s[i] with s[j]
						mov		r11b, byte [s + r9]
						mov		byte [s + r8], r11b
						mov		byte [s + r9], r10b
						inc		r8						; -
						cmp		r8, 256
						jl		permut_s_iteration

						ret

						; encrypt_with_keystream - generate keystrem and XOR plaintext with the keystream
						; rdi ~ plaintext
						; rsi ~ ciphertext
						; rdx ~ texts' length
encrypt_with_keystream:	push	rbx
						push	r12
						push	r13
						push	r14
						push	r15

						mov		r15, rdi 				; r15 ~ plaintext
						mov		r14, rsi 				; r14 ~ ciphertext
						mov		r13, rdx				; r13 ~ plaintext length

						mov		r8, 0					; r8 ~ i
						mov		r9, 0					; r9 ~ j
						mov		r10, 0					; r10 ~ n
encryption_iteration:	inc		r8						; i = (i + 1) % 256
						mov		rax, r8
						xor		rdx, rdx
						mov		rbx, 256
						div		rbx
						mov		r8, rdx
						add		r9b, byte [s + r8]		; j += s[i];
						mov		rax, r9					; j = j % 256
						xor		rdx, rdx
						mov		rbx, 256
						div		rbx
						mov		r9, rdx
						mov		r11b, byte [s + r8]		; swap s[i] with s[j]
						mov		r12b, byte [s + r9]
						mov		byte [s + r9], r11b
						mov		byte [s + r8], r12b
						xor		r11, r11 				; random byte index = s[i] + s[j]
						add		r11b, byte [s + r8]
						add		r11b, byte [s + r9]
						mov		rax, r11				; random byte index = random byte index % 256
						xor		rdx, rdx
						mov		rbx, 256
						div		rbx
						xor		r11, r11				; random byte = s[random byte index]	
						mov		r11b, byte [s + rdx]
						xor		r11b, byte [r15 + r10]	; encrypyted byte = plain text byte ^ random byte
						mov		byte [r14 + r10], r11b	; ciphertext[n] = encrypted byte
						inc		r10						; -
						cmp		r10, r13
						jl		encryption_iteration

						pop		r15
						pop		r14
						pop		r13
						pop		r12
						pop		rbx
						ret