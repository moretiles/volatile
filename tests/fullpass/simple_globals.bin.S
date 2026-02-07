.intel_syntax

.global main

.text
main:
// line #1
mov %r11, a[%rip]
mov %r10, 3
add %r11, %r10

// line #2
mov %r11, 4
mov %r10, a[%rip]
add %r11, %r10
mov %r10, b[%rip]
sub %r11, %r10

// line #3
mov %r11, a[%rip]
mov %r10, a[%rip]
add %r11, %r10
mov %r10, a[%rip]
add %r11, %r10

mov %rax, %r11
ret

.data
b: .long 3
a: .long 2
zero: .long 0
one: .long 1
