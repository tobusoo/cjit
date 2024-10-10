; *** IR Dump Before SinkingPass on test ***
; Function Attrs: noinline nounwind ssp uwtable(sync)
define double @test(ptr noundef %0, i32 noundef %1) #0 {
  br label %3

3:                                                ; preds = %29, %2
  %.014 = phi i32 [ 0, %2 ], [ %30, %29 ]
  %.0 = phi double [ 0.000000e+00, %2 ], [ %28, %29 ]
  %4 = icmp ult i32 %.014, %1
  br i1 %4, label %5, label %31, !prof !4

5:                                                ; preds = %3
  %6 = sext i32 %.014 to i64
  %7 = getelementptr inbounds i32, ptr %0, i64 %6
  %8 = load i32, ptr %7, align 4
  %9 = sitofp i32 %8 to double
  %10 = call double @llvm.sin.f64(double %9)
  %11 = sext i32 %.014 to i64
  %12 = getelementptr inbounds i32, ptr %0, i64 %11
  %13 = load i32, ptr %12, align 4
  %14 = sitofp i32 %13 to double
  %15 = call double @llvm.cos.f64(double %14)
  %16 = fmul double %10, %15
  %17 = sext i32 %.014 to i64
  %18 = getelementptr inbounds i32, ptr %0, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = icmp slt i32 %19, 0
  br i1 %20, label %21, label %23, !prof !5

21:                                               ; preds = %5
  %22 = fadd double %.0, %16
  br label %23

23:                                               ; preds = %21, %5
  %.1 = phi double [ %22, %21 ], [ %.0, %5 ]
  %24 = sext i32 %.014 to i64
  %25 = getelementptr inbounds i32, ptr %0, i64 %24
  %26 = load i32, ptr %25, align 4
  %27 = sitofp i32 %26 to double
  %28 = fadd double %.1, %27
  br label %29

29:                                               ; preds = %23
  %30 = add nsw i32 %.014, 1
  br label %3, !llvm.loop !6

31:                                               ; preds = %3
  ret double %.0
}
