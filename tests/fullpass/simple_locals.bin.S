.intel_syntax

.global main

.text
main:
// line #1
mov %r11, 20
mov %r10, main_c[%rbp]
sub %r11, %r10
mov %r10, 3
add %r11, %r10

// line #2
mov %r11, 5
mov %r10, 5
add %r11, %r10
mov %r10, main_d[%rbp]
add %r11, %r10

// line #3
mov %r11, main_c[%rbp]
mov %r10, main_d[%rbp]
add %r11, %r10
mov %r10, main_c[%rbp]
add %r11, %r10

mov %rax, %r11
ret

.data
b: .long 3
a: .long 2
zero: .long 0
one: .long 1
