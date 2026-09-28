; RUN: llc -mtriple=mips64-unknown-linux-gnuabin32 -mcpu=mips3 -target-abi n32 \
; RUN:   -relocation-model=pic -O2 < %s | FileCheck %s
; RUN: llc -mtriple=mips64-sgi-irix6.5 -mcpu=mips3 -target-abi n32 \
; RUN:   -relocation-model=pic -O2 < %s | FileCheck %s

; MipsSEFrameLowering::determineCalleeSaves expands the pseudos that spill
; the HI/LO accumulator (STORE_ACC64/LOAD_ACC64 here, around an sdiv/srem
; pair) into instructions with virtual registers. Shrink-wrapping calls it
; partway through its scan of a function and then asserted on those
; ("Unallocated register?!"). Reduced from libXfont2's BitmapOpenScalable.

; CHECK-LABEL: BitmapOpenScalable:
target datalayout = "E-m:e-p:32:32-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"

define i32 @BitmapOpenScalable(ptr %call.i90, ptr %0, ptr %firstRow109.i, ptr %info6.i, i32 %mul.i99, i32 %sub140.i, i32 %div.i100, ptr %call141.i, i16 %bf.load148.i, i1 %cmp151.i, ptr %fontAscent.i, ptr %1) #0 {
entry:
  %call.i901 = call ptr null()
  store ptr null, ptr %call.i90, align 4
  store ptr null, ptr %info6.i, align 4
  %conv106.i = zext i16 %bf.load148.i to i32
  %2 = load i16, ptr %call.i90, align 2
  %conv108.i = zext i16 %2 to i32
  %3 = load i16, ptr %firstRow109.i, align 4
  %conv110.i = zext i16 %3 to i32
  %4 = load i16, ptr null, align 2
  %call114.i = call ptr null(i32 0)
  %sub119.i = sub i32 %conv108.i, %conv106.i
  %mul.i994 = mul i32 %mul.i99, %sub119.i
  store i16 %3, ptr %call.i901, align 4
  store i16 %4, ptr null, align 2
  store i16 %bf.load148.i, ptr %call.i90, align 4
  store i16 %2, ptr null, align 2
  store i32 0, ptr %firstRow109.i, align 4
  store i32 %div.i100, ptr null, align 4
  call void @llvm.memset.p0.i64(ptr %fontAscent.i, i8 0, i64 20, i1 false)
  %call132.i = call ptr null(ptr null, i32 signext %mul.i994, i32 0)
  store ptr null, ptr null, align 4
  %call141.i7 = call ptr null(i32 signext %mul.i99, i32 1)
  store ptr %call.i90, ptr null, align 4
  store i16 %bf.load148.i, ptr null, align 2
  %call.i.i.i13 = call double @hypot()
  %call.i107.i.i = call double @hypot()
  br i1 %cmp151.i, label %if.then25, label %if.end166.i

if.end166.i:                                      ; preds = %entry
  %conv170.i = fptosi double 0x7FF8000000000000 to i16
  store i16 %conv170.i, ptr %0, align 4
  %5 = load i16, ptr %call.i90, align 2
  %conv488.i = sitofp i16 %5 to double
  %6 = call double @llvm.fmuladd.f64(double 0.000000e+00, double %conv488.i, double 0.000000e+00)
  %cmp573.i = fcmp ogt double 0.000000e+00, %6
  %newlsb.2.i = select i1 %cmp573.i, double 1.000000e+00, double 0.000000e+00
  %7 = call double @llvm.floor.f64(double %newlsb.2.i)
  %conv606.i = fptosi double %7 to i32
  %conv607.i = trunc i32 %conv606.i to i16
  store i16 %conv607.i, ptr null, align 4
  store i16 0, ptr %firstRow109.i, align 2
  br label %for.body691.i

for.body691.i:                                    ; preds = %for.inc974.i, %if.end166.i
  %i.21641.i = phi i32 [ 0, %if.end166.i ], [ 1, %for.inc974.i ]
  br i1 %cmp151.i, label %for.inc974.i, label %cond.end703.i

cond.end703.i:                                    ; preds = %for.body691.i
  br i1 %cmp151.i, label %for.inc974.i, label %land.lhs.true706.i

land.lhs.true706.i:                               ; preds = %cond.end703.i
  %div710.i = sdiv i32 %i.21641.i, %mul.i99
  %add711.i = or i32 %div710.i, %conv110.i
  %conv718.i = zext i16 %bf.load148.i to i32
  %mul724.i = mul i32 %conv718.i, %add711.i
  %rem727.i = srem i32 %i.21641.i, %mul.i99
  %add728.i = or i32 %rem727.i, %div.i100
  %sub733.i = or i32 %add728.i, %mul724.i
  %arrayidx735.i = getelementptr ptr, ptr null, i32 %sub733.i
  %tobool736.not.i = icmp eq ptr %arrayidx735.i, null
  br i1 %tobool736.not.i, label %for.inc974.i, label %cond.true737.i

cond.true737.i:                                   ; preds = %land.lhs.true706.i
  %arrayidx794.i = getelementptr ptr, ptr %1, i32 0
  br label %for.inc974.i

for.inc974.i:                                     ; preds = %cond.true737.i, %land.lhs.true706.i, %cond.end703.i, %for.body691.i
  %exitcond1650.not.i = icmp eq i32 0, %div.i100
  br i1 %exitcond1650.not.i, label %for.end976.i, label %for.body691.i

for.end976.i:                                     ; preds = %for.inc974.i
  %ink_minbounds.i = getelementptr i8, ptr %call.i90, i32 56
  br label %if.then25

if.then25:                                        ; preds = %for.end976.i, %entry
  ret i32 0
}

declare double @hypot()

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.floor.f64(double) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

; uselistorder directives
uselistorder ptr @hypot, { 1, 0 }

attributes #0 = { "frame-pointer"="all" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
