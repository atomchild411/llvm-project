; RUN: llc -mtriple=mips64 -target-abi n32 -relocation-model=pic < %s | FileCheck %s

; N32 keeps 32-bit values sign-extended in 64-bit registers. A pointer taken
; from the low half of an i64 (the second member of a two-pointer struct that
; N32 passes in one register) must be sign-extended before it is passed on or
; returned, or the callee sees the other member in the upper half.

declare ptr @memchr(ptr, i32 signext, i32 signext)

define ptr @pass_low_half(i64 %pair, i32 signext %c, i32 signext %n) {
; CHECK-LABEL: pass_low_half:
; CHECK:       jalr $25
; CHECK-NEXT:  sll $4, $4, 0
  %lo = trunc i64 %pair to i32
  %p = inttoptr i32 %lo to ptr
  %r = call ptr @memchr(ptr %p, i32 signext %c, i32 signext %n)
  ret ptr %r
}

define ptr @return_low_half(i64 %pair) {
; CHECK-LABEL: return_low_half:
; CHECK:       jr $ra
; CHECK-NEXT:  sll $2, $4, 0
  %lo = trunc i64 %pair to i32
  %p = inttoptr i32 %lo to ptr
  ret ptr %p
}
