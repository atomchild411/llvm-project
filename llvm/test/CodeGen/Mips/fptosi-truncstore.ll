; A 64-bit fptosi whose result is truncated and stored to 32 bits must store
; 4 bytes. lowerFP_TO_SINT_STORE used to rewrite the truncating store as a
; full 8-byte store of the trunc.l result: sdl/sdr when the field is 4-byte
; aligned, sdc1 when it is 8-byte aligned, either way writing 4 bytes past it.

; RUN: llc -mtriple=mips64-sgi-irix6.5 -target-abi=n32 -mcpu=mips4 < %s | FileCheck %s -check-prefixes=ALL,BE
; RUN: llc -mtriple=mips64 -target-abi=n64 -mcpu=mips3 < %s | FileCheck %s -check-prefixes=ALL,BE
; RUN: llc -mtriple=mips64 -target-abi=n64 -mcpu=mips64r2 < %s | FileCheck %s -check-prefixes=ALL,BE
; RUN: llc -mtriple=mips64el -target-abi=n64 -mcpu=mips64 < %s | FileCheck %s -check-prefixes=ALL,LE
; RUN: llc -mtriple=mips64 -target-abi=n64 -mcpu=mips64r6 < %s | FileCheck %s -check-prefixes=ALL,R6

@g32 = global i32 0, align 4
@g32a8 = global i32 0, align 8

; ALL-LABEL: double_to_i64_to_i32:
; ALL:       trunc.l.d $f[[F:[0-9]+]],
; ALL:       dmfc1 $[[R:[0-9]+]], $f[[F]]
; ALL-NOT:   sdl
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       sw $[[R]], 4($
; ALL-NOT:   sdl
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       .end double_to_i64_to_i32
define void @double_to_i64_to_i32(ptr %p, double %d) {
  %i = fptosi double %d to i64
  %t = trunc i64 %i to i32
  %f = getelementptr inbounds i8, ptr %p, i64 4
  store i32 %t, ptr %f, align 4
  ret void
}

; ALL-LABEL: float_to_i64_to_i32:
; ALL:       trunc.l.s $f[[F:[0-9]+]],
; ALL:       dmfc1 $[[R:[0-9]+]], $f[[F]]
; ALL-NOT:   sdl
; ALL-NOT:   sdc1
; ALL:       sw $[[R]], 0($
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       .end float_to_i64_to_i32
define void @float_to_i64_to_i32(ptr %p, float %d) {
  %i = fptosi float %d to i64
  %t = trunc i64 %i to i32
  store i32 %t, ptr %p, align 4
  ret void
}

; ALL-LABEL: volatile_store:
; ALL:       trunc.l.d
; ALL-NOT:   sdl
; ALL-NOT:   sdc1
; ALL:       sw $
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       .end volatile_store
define void @volatile_store(ptr %p, double %d) {
  %i = fptosi double %d to i64
  %t = trunc i64 %i to i32
  store volatile i32 %t, ptr %p, align 4
  ret void
}

; An 8-byte aligned destination used to get an sdc1 instead of sdl/sdr.
; ALL-LABEL: aligned8_global:
; ALL:       trunc.l.d
; ALL-NOT:   sdc1
; ALL:       sw $
; ALL-NOT:   sdc1
; ALL:       .end aligned8_global
define void @aligned8_global(double %d) {
  %i = fptosi double %d to i64
  %t = trunc i64 %i to i32
  store i32 %t, ptr @g32a8, align 8
  ret void
}

; ALL-LABEL: global_store:
; ALL:       trunc.l.d
; ALL-NOT:   sdl
; ALL-NOT:   sdc1
; ALL:       sw $
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       .end global_store
define void @global_store(double %d) {
  %i = fptosi double %d to i64
  %t = trunc i64 %i to i32
  store i32 %t, ptr @g32, align 4
  ret void
}

; An unaligned 32-bit destination takes a 32-bit swl/swr pair (sw on r6).
; ALL-LABEL: unaligned_store:
; ALL:       trunc.l.d
; ALL-NOT:   sdl
; ALL-NOT:   sdc1
; BE:        swl $[[R:[0-9]+]], 0($[[P:[0-9]+]])
; BE:        swr $[[R]], 3($[[P]])
; LE:        swl $[[R:[0-9]+]], 3($[[P:[0-9]+]])
; LE:        swr $[[R]], 0($[[P]])
; R6:        sw $
; ALL-NOT:   sdr
; ALL-NOT:   sdc1
; ALL:       .end unaligned_store
define void @unaligned_store(ptr %p, double %d) {
  %i = fptosi double %d to i64
  %t = trunc i64 %i to i32
  store i32 %t, ptr %p, align 1
  ret void
}

; Not a truncating store: the 8-byte store of the trunc.l result stays.
; ALL-LABEL: i64_store:
; ALL:       trunc.l.d $f[[F:[0-9]+]],
; ALL:       sdc1 $f[[F]], 0($
; ALL:       .end i64_store
define void @i64_store(ptr %p, double %d) {
  %i = fptosi double %d to i64
  store i64 %i, ptr %p, align 8
  ret void
}
