.intel_syntax

.text
// line #3, indentation 1
	main:
	push %rbp
	push %rbx
	push %r12
	push %r13
	push %r14
	push %r15
	mov %rbp, %rsp
	sub %rsp, 0x400


// line #4, indentation 1
	mov %r11, 48
	mov -8[%rbp], %r11

// line #5, indentation 1
	mov %r11, 7
	mov -16[%rbp], %r11

// line #6, indentation 1
	mov %r11, 51
	mov -24[%rbp], %r11

// line #7, indentation 1

// line #8, indentation 1

// line #9, indentation 1
	lea %r11, my_interned_string[%rip]
	mov -32[%rbp], %r11

// line #10, indentation 1
	mov %r11, -32[%rbp]
	mov %rdi, %r11
	nop
	nop
	call puts@PLT

// line #11, indentation 1

// line #12, indentation 1
	mov %r11, 0
	mov %rax, %r11
	mov %rsp, %rbp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbx
	pop %rbp
	ret

// line #13, indentation 1
	16:


.global main
.type main, function
.global puts
.type puts, function

.data
.global my_interned_string
.type my_interned_string, gnu_unique_object
my_interned_string: .asciz "Hello world!"
