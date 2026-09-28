; RUN: llc -mtriple=mips64 -target-abi n32 -O2 < %s | FileCheck %s
; RUN: llc -mtriple=mips64 -target-abi n64 -O2 < %s | FileCheck %s

; A value live across blocks gets an AssertSext for the sign bits SelectionDAG
; knows of it: here 29 (the high words are 0xfffffffb and 0xfffffff9), i.e.
; AssertSext from i36. That says nothing about bit 31, so truncating it to i32
; is not free: it needs an sll to sign-extend the low word. Using the 64-bit
; register as is made a 32-bit pointer from the whole value (JSC's 32-bit
; JSValue: tag in the high word, cell pointer in the low word).

define i8 @load_through_low_word(i1 %c, i32 %p, i32 %q) {
; CHECK-LABEL: load_through_low_word:
; CHECK:       sll [[PTR:\$[0-9]+]], {{\$[0-9]+}}, 0
; CHECK:       lbu {{\$[0-9]+}}, 5([[PTR]])
entry:
  %zp = zext i32 %p to i64
  %a = or disjoint i64 %zp, -21474836480
  br i1 %c, label %other, label %join

other:
  %zq = zext i32 %q to i64
  %b = or disjoint i64 %zq, -30064771072
  br label %join

join:
  %v = phi i64 [ %a, %entry ], [ %b, %other ]
  %lo = trunc i64 %v to i32
  %ptr = inttoptr i32 %lo to ptr
  %field = getelementptr i8, ptr %ptr, i32 5
  %r = load i8, ptr %field
  ret i8 %r
}
