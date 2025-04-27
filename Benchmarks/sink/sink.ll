; ModuleID = 'sink.c'
source_filename = "sink.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local double @sink(ptr noundef %arr, i32 noundef %n) #0 {
entry:
  %arr.addr = alloca ptr, align 8
  %n.addr = alloca i32, align 4
  %res = alloca double, align 8
  %i = alloca i32, align 4
  %partial_res = alloca double, align 8
  store ptr %arr, ptr %arr.addr, align 8
  store i32 %n, ptr %n.addr, align 4
  store double 0.000000e+00, ptr %res, align 8
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %n.addr, align 4
  %cmp = icmp ult i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load ptr, ptr %arr.addr, align 8
  %3 = load i32, ptr %i, align 4
  %idxprom = sext i32 %3 to i64
  %arrayidx = getelementptr inbounds i32, ptr %2, i64 %idxprom
  %4 = load i32, ptr %arrayidx, align 4
  %conv = sitofp i32 %4 to double
  %call = call double @sin(double noundef %conv) #2
  %5 = load ptr, ptr %arr.addr, align 8
  %6 = load i32, ptr %i, align 4
  %idxprom1 = sext i32 %6 to i64
  %arrayidx2 = getelementptr inbounds i32, ptr %5, i64 %idxprom1
  %7 = load i32, ptr %arrayidx2, align 4
  %conv3 = sitofp i32 %7 to double
  %call4 = call double @cos(double noundef %conv3) #2
  %mul = fmul double %call, %call4
  store double %mul, ptr %partial_res, align 8
  %8 = load ptr, ptr %arr.addr, align 8
  %9 = load i32, ptr %i, align 4
  %idxprom5 = sext i32 %9 to i64
  %arrayidx6 = getelementptr inbounds i32, ptr %8, i64 %idxprom5
  %10 = load i32, ptr %arrayidx6, align 4
  %cmp7 = icmp slt i32 %10, 0
  br i1 %cmp7, label %if.then, label %if.end

if.then:                                          ; preds = %for.body
  %11 = load double, ptr %partial_res, align 8
  %12 = load double, ptr %res, align 8
  %add = fadd double %12, %11
  store double %add, ptr %res, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %for.body
  %13 = load ptr, ptr %arr.addr, align 8
  %14 = load i32, ptr %i, align 4
  %idxprom9 = sext i32 %14 to i64
  %arrayidx10 = getelementptr inbounds i32, ptr %13, i64 %idxprom9
  %15 = load i32, ptr %arrayidx10, align 4
  %conv11 = sitofp i32 %15 to double
  %16 = load double, ptr %res, align 8
  %add12 = fadd double %16, %conv11
  store double %add12, ptr %res, align 8
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %17 = load i32, ptr %i, align 4
  %inc = add nsw i32 %17, 1
  store i32 %inc, ptr %i, align 4
  br label %for.cond, !llvm.loop !6

for.end:                                          ; preds = %for.cond
  %18 = load double, ptr %res, align 8
  ret double %18
}

; Function Attrs: nounwind
declare double @sin(double noundef) #1

; Function Attrs: nounwind
declare double @cos(double noundef) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.0 (https://github.com/llvm/llvm-project.git a4bf6cd7cfb1a1421ba92bca9d017b49936c55e4)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
