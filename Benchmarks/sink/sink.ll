; ModuleID = 'sink.c'
source_filename = "sink.c"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define double @test(ptr noundef %0, i32 noundef %1) #0 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca double, align 8
  %6 = alloca i32, align 4
  %7 = alloca double, align 8
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  store double 0.000000e+00, ptr %5, align 8
  store i32 0, ptr %6, align 4
  br label %8

8:                                                ; preds = %47, %2
  %9 = load i32, ptr %6, align 4
  %10 = load i32, ptr %4, align 4
  %11 = icmp ult i32 %9, %10
  br i1 %11, label %12, label %50, !prof !6

12:                                               ; preds = %8
  %13 = load ptr, ptr %3, align 8
  %14 = load i32, ptr %6, align 4
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds i32, ptr %13, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = sitofp i32 %17 to double
  %19 = call double @llvm.sin.f64(double %18)
  %20 = load ptr, ptr %3, align 8
  %21 = load i32, ptr %6, align 4
  %22 = sext i32 %21 to i64
  %23 = getelementptr inbounds i32, ptr %20, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = sitofp i32 %24 to double
  %26 = call double @llvm.cos.f64(double %25)
  %27 = fmul double %19, %26
  store double %27, ptr %7, align 8
  %28 = load ptr, ptr %3, align 8
  %29 = load i32, ptr %6, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds i32, ptr %28, i64 %30
  %32 = load i32, ptr %31, align 4
  %33 = icmp slt i32 %32, 0
  br i1 %33, label %34, label %38, !prof !7

34:                                               ; preds = %12
  %35 = load double, ptr %7, align 8
  %36 = load double, ptr %5, align 8
  %37 = fadd double %36, %35
  store double %37, ptr %5, align 8
  br label %38

38:                                               ; preds = %34, %12
  %39 = load ptr, ptr %3, align 8
  %40 = load i32, ptr %6, align 4
  %41 = sext i32 %40 to i64
  %42 = getelementptr inbounds i32, ptr %39, i64 %41
  %43 = load i32, ptr %42, align 4
  %44 = sitofp i32 %43 to double
  %45 = load double, ptr %5, align 8
  %46 = fadd double %45, %44
  store double %46, ptr %5, align 8
  br label %47

47:                                               ; preds = %38
  %48 = load i32, ptr %6, align 4
  %49 = add nsw i32 %48, 1
  store i32 %49, ptr %6, align 4
  br label %8, !llvm.loop !5

50:                                               ; preds = %8
  %51 = load double, ptr %5, align 8
  ret double %51
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
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = !{!"branch_weights", i32 10000, i32 1}
!7 = !{!"branch_weights", i32 1, i32 2147483647}
