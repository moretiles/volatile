.intel_syntax

.global main

.text
main:
// line #1
mov %r11, 5
mov %r10, 4
add %r11, %r10

// line #2
mov %r11, 3
mov %r10, 1
sub %r11, %r10

// line #3
mov %r11, 2
mov %r10, 3
add %r11, %r10

mov %rax, %r11
ret

.data
b: .long 3
a: .long 2
zero: .long 0
one: .long 1
