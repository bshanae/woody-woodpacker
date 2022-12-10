global				main
extern				printf


section				.data

literal_s:			db		"s:", 0xa, 0
literal_nl:			db		0xa, 0
format_str_nl:		db		"%s", 0xa, 0
format_hex:			db		"%x", 0
format_hex_nl:		db		"%x", 0xa, 0
format_hex2_nl:		db		"%x, %x", 0xa, 0
format_hex3_nl:		db		"%x, %x, %x", 0xa, 0
format_dec2_nl:		db		"%d, %d", 0xa, 0
format_mark:		db		"M%i", 0xa, 0



section				.text
print_s:			mov     rdi, literal_s
			        call    printf
					xor		rcx, rcx
print_s_iteration:	push    rcx
        			mov     rdi, format_hex
        			xor		rsi, rsi
        			mov     sil, byte [s + rcx]
			        call    printf
			        pop     rcx
			        inc		rcx
			        cmp		rcx, 256
			        jl		print_s_iteration

			        mov     rdi, literal_nl
			        call    printf
			        mov     rdi, literal_nl
			        call    printf

			        ret

mark:				mov		rsi, rdi
					mov     rdi, format_mark
					call    printf
			        ret

exit:				mov       rax, 60
          			xor       rdi, rdi
          			syscall
