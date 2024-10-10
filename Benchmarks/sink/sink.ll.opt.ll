; ModuleID = 'Benchmarks/sink/sink.ll'
source_filename = "sink.c"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define double @test(ptr noundef %0, i32 noundef %1) #0 {
  br label %3

3:                                                ; preds = %24, %2
  %.014 = phi i32 [ 0, %2 ], [ %30, %24 ]
  %.0 = phi double [ 0.000000e+00, %2 ], [ %29, %24 ]
  %4 = icmp ult i32 %.014, %1
  br i1 %4, label %5, label %31, !prof !4

5:                                                ; preds = %3
  %6 = sext i32 %.014 to i64
  %7 = getelementptr inbounds i32, ptr %0, i64 %6
  %8 = load i32, ptr %7, align 4
  %9 = icmp slt i32 %8, 0
  br i1 %9, label %10, label %23, !prof !5

10:                                               ; preds = %5
  %11 = sext i32 %.014 to i64
  %12 = getelementptr inbounds i32, ptr %0, i64 %11
  %13 = load i32, ptr %12, align 4
  %14 = sitofp i32 %13 to double
  %15 = call double @llvm.sin.f64(double %14)
  %16 = sext i32 %.014 to i64
  %17 = getelementptr inbounds i32, ptr %0, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = sitofp i32 %18 to double
  %20 = call double @llvm.cos.f64(double %19)
  %21 = fmul double %15, %20
  %22 = fadd double %.0, %21
  br label %23

23:                                               ; preds = %10, %5
  %.1 = phi double [ %22, %10 ], [ %.0, %5 ]
  br label %24

24:                                               ; preds = %23
  %25 = sext i32 %.014 to i64
  %26 = getelementptr inbounds i32, ptr %0, i64 %25
  %27 = load i32, ptr %26, align 4
  %28 = sitofp i32 %27 to double
  %29 = fadd double %.1, %28
  %30 = add nsw i32 %.014, 1
  br label %3, !llvm.loop !6

31:                                               ; preds = %3
  ret double %.0
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.sin.f64(double) #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.cos.f64(double) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"branch_weights", i32 10000, i32 1}
!5 = !{!"branch_weights", i32 1, i32 2147483647}
!6 = !{!"llvm.loop.mustprogress"}
