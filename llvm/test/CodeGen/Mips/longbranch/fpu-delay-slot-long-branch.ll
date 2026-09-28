; On MIPS I-III an FPU compare needs a nop before the bc1t/bc1f that reads its
; condition. Those nops are inserted after branch ranges are first checked;
; here they alone push the branch around %big out of range, which must then
; become a long branch rather than an out-of-range PC16 fixup.

; RUN: llc -mtriple=mips64-unknown-linux-gnu -mcpu=mips3 -target-abi n32 \
; RUN:   -relocation-model=pic < %s | FileCheck %s
; RUN: llc -mtriple=mips64-unknown-linux-gnu -mcpu=mips3 -target-abi n32 \
; RUN:   -relocation-model=pic -filetype=obj < %s -o /dev/null

; CHECK-LABEL: f:
; CHECK:       bnez $4, [[BIG:\.LBB0_[0-9]+]]
; CHECK:       lui $1, %hi([[FAR:\.LBB0_[0-9]+]]-[[BALTGT:\.LBB0_[0-9]+]])
; CHECK-NEXT:  bal [[BALTGT]]
; CHECK:       jr $1
; CHECK:       [[BIG]]:
; CHECK:       .space 130400
; CHECK:       c.olt.d
; CHECK-NEXT:  nop
; CHECK-NEXT:  bc1t

define void @f(i32 signext %n, double %a, double %b0, double %b1, double %b2, double %b3, double %b4, double %b5, double %b6, double %b7, double %b8, double %b9, double %b10, double %b11, double %b12, double %b13, double %b14, double %b15, double %b16, double %b17, double %b18, double %b19, double %b20, double %b21, double %b22, double %b23, double %b24, double %b25, double %b26, double %b27, double %b28, double %b29, double %b30, double %b31, double %b32, double %b33, double %b34, double %b35, double %b36, double %b37, double %b38, double %b39) {
entry:
  %z = icmp eq i32 %n, 0
  br i1 %z, label %far, label %big

big:
  call void asm sideeffect ".space 130400", ""()
  br label %bb0

bb0:
  %c0 = fcmp olt double %a, %b0
  br i1 %c0, label %side, label %bb1
bb1:
  %c1 = fcmp olt double %a, %b1
  br i1 %c1, label %side, label %bb2
bb2:
  %c2 = fcmp olt double %a, %b2
  br i1 %c2, label %side, label %bb3
bb3:
  %c3 = fcmp olt double %a, %b3
  br i1 %c3, label %side, label %bb4
bb4:
  %c4 = fcmp olt double %a, %b4
  br i1 %c4, label %side, label %bb5
bb5:
  %c5 = fcmp olt double %a, %b5
  br i1 %c5, label %side, label %bb6
bb6:
  %c6 = fcmp olt double %a, %b6
  br i1 %c6, label %side, label %bb7
bb7:
  %c7 = fcmp olt double %a, %b7
  br i1 %c7, label %side, label %bb8
bb8:
  %c8 = fcmp olt double %a, %b8
  br i1 %c8, label %side, label %bb9
bb9:
  %c9 = fcmp olt double %a, %b9
  br i1 %c9, label %side, label %bb10
bb10:
  %c10 = fcmp olt double %a, %b10
  br i1 %c10, label %side, label %bb11
bb11:
  %c11 = fcmp olt double %a, %b11
  br i1 %c11, label %side, label %bb12
bb12:
  %c12 = fcmp olt double %a, %b12
  br i1 %c12, label %side, label %bb13
bb13:
  %c13 = fcmp olt double %a, %b13
  br i1 %c13, label %side, label %bb14
bb14:
  %c14 = fcmp olt double %a, %b14
  br i1 %c14, label %side, label %bb15
bb15:
  %c15 = fcmp olt double %a, %b15
  br i1 %c15, label %side, label %bb16
bb16:
  %c16 = fcmp olt double %a, %b16
  br i1 %c16, label %side, label %bb17
bb17:
  %c17 = fcmp olt double %a, %b17
  br i1 %c17, label %side, label %bb18
bb18:
  %c18 = fcmp olt double %a, %b18
  br i1 %c18, label %side, label %bb19
bb19:
  %c19 = fcmp olt double %a, %b19
  br i1 %c19, label %side, label %bb20
bb20:
  %c20 = fcmp olt double %a, %b20
  br i1 %c20, label %side, label %bb21
bb21:
  %c21 = fcmp olt double %a, %b21
  br i1 %c21, label %side, label %bb22
bb22:
  %c22 = fcmp olt double %a, %b22
  br i1 %c22, label %side, label %bb23
bb23:
  %c23 = fcmp olt double %a, %b23
  br i1 %c23, label %side, label %bb24
bb24:
  %c24 = fcmp olt double %a, %b24
  br i1 %c24, label %side, label %bb25
bb25:
  %c25 = fcmp olt double %a, %b25
  br i1 %c25, label %side, label %bb26
bb26:
  %c26 = fcmp olt double %a, %b26
  br i1 %c26, label %side, label %bb27
bb27:
  %c27 = fcmp olt double %a, %b27
  br i1 %c27, label %side, label %bb28
bb28:
  %c28 = fcmp olt double %a, %b28
  br i1 %c28, label %side, label %bb29
bb29:
  %c29 = fcmp olt double %a, %b29
  br i1 %c29, label %side, label %bb30
bb30:
  %c30 = fcmp olt double %a, %b30
  br i1 %c30, label %side, label %bb31
bb31:
  %c31 = fcmp olt double %a, %b31
  br i1 %c31, label %side, label %bb32
bb32:
  %c32 = fcmp olt double %a, %b32
  br i1 %c32, label %side, label %bb33
bb33:
  %c33 = fcmp olt double %a, %b33
  br i1 %c33, label %side, label %bb34
bb34:
  %c34 = fcmp olt double %a, %b34
  br i1 %c34, label %side, label %bb35
bb35:
  %c35 = fcmp olt double %a, %b35
  br i1 %c35, label %side, label %bb36
bb36:
  %c36 = fcmp olt double %a, %b36
  br i1 %c36, label %side, label %bb37
bb37:
  %c37 = fcmp olt double %a, %b37
  br i1 %c37, label %side, label %bb38
bb38:
  %c38 = fcmp olt double %a, %b38
  br i1 %c38, label %side, label %bb39
bb39:
  %c39 = fcmp olt double %a, %b39
  br i1 %c39, label %side, label %bb40
bb40:
  br label %far

side:
  call void asm sideeffect "nop", ""()
  br label %far

far:
  ret void
}
