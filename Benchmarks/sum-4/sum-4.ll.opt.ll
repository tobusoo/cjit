; ModuleID = 'Benchmarks/sum-4/sum-4.ll'
source_filename = "sum-4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@seed = internal global i64 0, align 8
@size = internal global i32 0, align 4
@ima = internal global ptr null, align 8
@imb = internal global ptr null, align 8
@imr = internal global ptr null, align 8

; Function Attrs: nounwind uwtable
define dso_local void @Initrand() #0 {
  store i64 8232, ptr @seed, align 8, !tbaa !5
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32 @Rand() #0 {
  %1 = load i64, ptr @seed, align 8, !tbaa !5
  %2 = mul nsw i64 %1, 1309
  %3 = add nsw i64 %2, 13849
  %4 = and i64 %3, 65535
  store i64 %4, ptr @seed, align 8, !tbaa !5
  %5 = trunc nuw nsw i64 %4 to i32
  ret i32 %5
}

; Function Attrs: nounwind uwtable
define dso_local void @InitArray(ptr noundef %0, i32 noundef %1) #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %.lr.ph, label %13

.lr.ph:                                           ; preds = %2
  %seed.promoted = load i64, ptr @seed, align 8, !tbaa !5
  %wide.trip.count = zext nneg i32 %1 to i64
  %4 = freeze i64 %seed.promoted
  br label %5

5:                                                ; preds = %.lr.ph, %5
  %indvars.iv = phi i64 [ 0, %.lr.ph ], [ %indvars.iv.next, %5 ]
  %.fr = phi i64 [ %4, %.lr.ph ], [ %8, %5 ]
  %6 = mul i64 %.fr, 1309
  %7 = add i64 %6, 13849
  %8 = and i64 %7, 65535
  %9 = trunc nuw nsw i64 %8 to i32
  %10 = urem i32 %9, 120
  %11 = add nsw i32 %10, -60
  %12 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv
  store i32 %11, ptr %12, align 4, !tbaa !9
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %._crit_edge, label %5, !llvm.loop !11

._crit_edge:                                      ; preds = %5
  store i64 %8, ptr @seed, align 8, !tbaa !5
  br label %13

13:                                               ; preds = %._crit_edge, %2
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nounwind uwtable
define dso_local i32 @init(i32 noundef %0) #0 {
  store i32 %0, ptr @size, align 4, !tbaa !9
  %2 = sext i32 %0 to i64
  %3 = call ptr @calloc(i64 noundef %2, i64 noundef 4) #6
  store ptr %3, ptr @ima, align 8, !tbaa !14
  %.not = icmp eq ptr %3, null
  br i1 %.not, label %InitArray.exit1, label %4

4:                                                ; preds = %1
  %5 = call ptr @calloc(i64 noundef %2, i64 noundef 4) #6
  store ptr %5, ptr @imb, align 8, !tbaa !14
  %.not21 = icmp eq ptr %5, null
  br i1 %.not21, label %6, label %8

6:                                                ; preds = %4
  %7 = load ptr, ptr @ima, align 8, !tbaa !14
  call void @free(ptr noundef %7) #7
  br label %InitArray.exit1

8:                                                ; preds = %4
  %9 = call ptr @calloc(i64 noundef %2, i64 noundef 4) #6
  store ptr %9, ptr @imr, align 8, !tbaa !14
  %.not22 = icmp eq ptr %9, null
  br i1 %.not22, label %10, label %13

10:                                               ; preds = %8
  %11 = load ptr, ptr @ima, align 8, !tbaa !14
  call void @free(ptr noundef %11) #7
  %12 = load ptr, ptr @imb, align 8, !tbaa !14
  call void @free(ptr noundef %12) #7
  br label %InitArray.exit1

13:                                               ; preds = %8
  store i64 8232, ptr @seed, align 8, !tbaa !5
  %14 = load ptr, ptr @ima, align 8, !tbaa !14
  %15 = load i32, ptr @size, align 4, !tbaa !9
  %16 = icmp sgt i32 %15, 0
  br i1 %16, label %.lr.ph, label %InitArray.exit

.lr.ph:                                           ; preds = %13
  %wide.trip.count = zext nneg i32 %15 to i64
  br label %17

17:                                               ; preds = %.lr.ph, %17
  %indvars.iv = phi i64 [ 0, %.lr.ph ], [ %indvars.iv.next, %17 ]
  %18 = phi i64 [ 8232, %.lr.ph ], [ %21, %17 ]
  %19 = mul nuw nsw i64 %18, 1309
  %20 = add nuw nsw i64 %19, 13849
  %21 = and i64 %20, 65535
  %22 = trunc nuw nsw i64 %21 to i32
  %23 = urem i32 %22, 120
  %24 = add nsw i32 %23, -60
  %25 = getelementptr inbounds i32, ptr %14, i64 %indvars.iv
  store i32 %24, ptr %25, align 4, !tbaa !9
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %.InitArray.exit_crit_edge, label %17, !llvm.loop !11

.InitArray.exit_crit_edge:                        ; preds = %17
  store i64 %21, ptr @seed, align 8, !tbaa !5
  %.pre = load i32, ptr @size, align 4, !tbaa !9
  br label %InitArray.exit

InitArray.exit:                                   ; preds = %.InitArray.exit_crit_edge, %13
  %seed.promoted34 = phi i64 [ %21, %.InitArray.exit_crit_edge ], [ 8232, %13 ]
  %26 = phi i32 [ %.pre, %.InitArray.exit_crit_edge ], [ %15, %13 ]
  %27 = load ptr, ptr @imb, align 8, !tbaa !14
  %28 = icmp sgt i32 %26, 0
  br i1 %28, label %.lr.ph33, label %InitArray.exit1

.lr.ph33:                                         ; preds = %InitArray.exit
  %wide.trip.count39 = zext nneg i32 %26 to i64
  br label %29

29:                                               ; preds = %.lr.ph33, %29
  %indvars.iv36 = phi i64 [ 0, %.lr.ph33 ], [ %indvars.iv.next37, %29 ]
  %.fr = phi i64 [ %seed.promoted34, %.lr.ph33 ], [ %32, %29 ]
  %30 = mul nsw i64 %.fr, 1309
  %31 = add nsw i64 %30, 13849
  %32 = and i64 %31, 65535
  %33 = trunc nuw nsw i64 %32 to i32
  %34 = urem i32 %33, 120
  %35 = add nsw i32 %34, -60
  %36 = getelementptr inbounds i32, ptr %27, i64 %indvars.iv36
  store i32 %35, ptr %36, align 4, !tbaa !9
  %indvars.iv.next37 = add nuw nsw i64 %indvars.iv36, 1
  %exitcond40.not = icmp eq i64 %indvars.iv.next37, %wide.trip.count39
  br i1 %exitcond40.not, label %.InitArray.exit1_crit_edge, label %29, !llvm.loop !11

.InitArray.exit1_crit_edge:                       ; preds = %29
  store i64 %32, ptr @seed, align 8, !tbaa !5
  br label %InitArray.exit1

InitArray.exit1:                                  ; preds = %InitArray.exit, %.InitArray.exit1_crit_edge, %1, %10, %6
  %.019 = phi i32 [ 0, %10 ], [ 0, %6 ], [ 0, %1 ], [ 1, %.InitArray.exit1_crit_edge ], [ 1, %InitArray.exit ]
  ret i32 %.019
}

; Function Attrs: allocsize(0,1)
declare ptr @calloc(i64 noundef, i64 noundef) #2

declare void @free(ptr noundef) #3

; Function Attrs: nounwind uwtable
define dso_local void @deinit() #0 {
  %1 = load ptr, ptr @ima, align 8, !tbaa !14
  call void @free(ptr noundef %1) #7
  %2 = load ptr, ptr @imb, align 8, !tbaa !14
  call void @free(ptr noundef %2) #7
  %3 = load ptr, ptr @imr, align 8, !tbaa !14
  call void @free(ptr noundef %3) #7
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32 @read_arr(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = icmp ne ptr %0, null
  %5 = icmp sgt i32 %2, -1
  %or.cond.not9 = and i1 %4, %5
  %.not = icmp slt i32 %2, %1
  %or.cond6 = and i1 %or.cond.not9, %.not
  br i1 %or.cond6, label %7, label %6

6:                                                ; preds = %3
  call void @abort() #8
  unreachable

7:                                                ; preds = %3
  %8 = zext nneg i32 %2 to i64
  %9 = getelementptr inbounds i32, ptr %0, i64 %8
  %10 = load i32, ptr %9, align 4, !tbaa !9
  ret i32 %10
}

; Function Attrs: noreturn nounwind
declare void @abort() #4

; Function Attrs: nounwind uwtable
define dso_local void @write_arr(ptr noundef %0, i32 noundef %1, i32 noundef %2, i32 noundef %3) #0 {
  %5 = icmp ne ptr %0, null
  %6 = icmp sgt i32 %2, -1
  %or.cond.not10 = and i1 %5, %6
  %.not = icmp slt i32 %2, %1
  %or.cond7 = and i1 %or.cond.not10, %.not
  br i1 %or.cond7, label %8, label %7

7:                                                ; preds = %4
  call void @abort() #8
  unreachable

8:                                                ; preds = %4
  %9 = zext nneg i32 %2 to i64
  %10 = getelementptr inbounds i32, ptr %0, i64 %9
  store i32 %3, ptr %10, align 4, !tbaa !9
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32 @sum_4() #0 {
  %1 = load i32, ptr @size, align 4, !tbaa !9
  %2 = icmp sgt i32 %1, 0
  br i1 %2, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %0
  %3 = load ptr, ptr @ima, align 8, !tbaa !14
  %4 = ptrtoint ptr %3 to i64
  %5 = icmp eq ptr %3, null
  %6 = load ptr, ptr @imb, align 8
  %.fr75 = freeze ptr %6
  %.fr7577 = ptrtoint ptr %.fr75 to i64
  %7 = load ptr, ptr @imr, align 8
  %.fr = freeze ptr %7
  %.fr76 = ptrtoint ptr %.fr to i64
  %8 = icmp eq ptr %.fr, null
  br i1 %5, label %.split, label %.lr.ph.split

.lr.ph.split:                                     ; preds = %.lr.ph
  %9 = icmp eq ptr %.fr75, null
  br i1 %9, label %read_arr.exit1.us, label %.lr.ph.split.split

read_arr.exit1.us:                                ; preds = %.lr.ph.split
  call void @abort() #8
  unreachable

.lr.ph.split.split:                               ; preds = %.lr.ph.split
  br i1 %8, label %read_arr.exit1.us63, label %.lr.ph.split.split.split.split

read_arr.exit1.us63:                              ; preds = %.lr.ph.split.split
  call void @abort() #8
  unreachable

.lr.ph.split.split.split.split:                   ; preds = %.lr.ph.split.split
  %wide.trip.count = zext nneg i32 %1 to i64
  %min.iters.check = icmp ult i32 %1, 8
  br i1 %min.iters.check, label %scalar.ph, label %vector.memcheck

vector.memcheck:                                  ; preds = %.lr.ph.split.split.split.split
  %10 = sub i64 %.fr76, %4
  %diff.check = icmp ult i64 %10, 128
  %11 = sub i64 %.fr76, %.fr7577
  %diff.check78 = icmp ult i64 %11, 128
  %conflict.rdx = or i1 %diff.check, %diff.check78
  br i1 %conflict.rdx, label %scalar.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.memcheck
  %n.vec = and i64 %wide.trip.count, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %16, %vector.body ]
  %12 = getelementptr inbounds i32, ptr %3, i64 %index
  %wide.load = load <8 x i32>, ptr %12, align 4, !tbaa !9
  %13 = getelementptr inbounds i32, ptr %.fr75, i64 %index
  %wide.load79 = load <8 x i32>, ptr %13, align 4, !tbaa !9
  %14 = add nsw <8 x i32> %wide.load, %wide.load79
  %15 = getelementptr inbounds i32, ptr %.fr, i64 %index
  store <8 x i32> %14, ptr %15, align 4, !tbaa !9
  %16 = add <8 x i32> %vec.phi, %14
  %index.next = add nuw i64 %index, 8
  %17 = icmp eq i64 %index.next, %n.vec
  br i1 %17, label %middle.block, label %vector.body, !llvm.loop !16

middle.block:                                     ; preds = %vector.body
  %18 = call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %16)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count
  br i1 %cmp.n, label %._crit_edge, label %scalar.ph

scalar.ph:                                        ; preds = %middle.block, %vector.memcheck, %.lr.ph.split.split.split.split
  %bc.resume.val = phi i64 [ %n.vec, %middle.block ], [ 0, %.lr.ph.split.split.split.split ], [ 0, %vector.memcheck ]
  %bc.merge.rdx = phi i32 [ %18, %middle.block ], [ 0, %.lr.ph.split.split.split.split ], [ 0, %vector.memcheck ]
  br label %read_arr.exit1

._crit_edge:                                      ; preds = %read_arr.exit1, %middle.block, %0
  %.0.lcssa = phi i32 [ 0, %0 ], [ %25, %read_arr.exit1 ], [ %18, %middle.block ]
  ret i32 %.0.lcssa

read_arr.exit1:                                   ; preds = %scalar.ph, %read_arr.exit1
  %indvars.iv = phi i64 [ %bc.resume.val, %scalar.ph ], [ %indvars.iv.next, %read_arr.exit1 ]
  %.055 = phi i32 [ %bc.merge.rdx, %scalar.ph ], [ %25, %read_arr.exit1 ]
  %19 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv
  %20 = load i32, ptr %19, align 4, !tbaa !9
  %21 = getelementptr inbounds i32, ptr %.fr75, i64 %indvars.iv
  %22 = load i32, ptr %21, align 4, !tbaa !9
  %23 = add nsw i32 %20, %22
  %24 = getelementptr inbounds i32, ptr %.fr, i64 %indvars.iv
  store i32 %23, ptr %24, align 4, !tbaa !9
  %25 = add i32 %.055, %23
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %._crit_edge, label %read_arr.exit1, !llvm.loop !19

.split:                                           ; preds = %.lr.ph
  call void @abort() #8
  unreachable
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v8i32(<8 x i32>) #5

attributes #0 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="znver3" "target-features"="+prfchw,-cldemote,+avx,+aes,+sahf,+pclmul,-xop,+crc32,+xsaves,-avx512fp16,-usermsr,-sm4,-egpr,+sse4.1,-avx512ifma,+xsave,+sse4.2,-tsxldtrk,-sm3,-ptwrite,-widekl,+invpcid,+64bit,+xsavec,-avx10.1-512,-avx512vpopcntdq,+cmov,-avx512vp2intersect,-avx512cd,+movbe,-avxvnniint8,-ccmp,-amx-int8,-kl,-avx10.1-256,-sha512,-avxvnni,-rtm,+adx,+avx2,-hreset,-movdiri,-serialize,+vpclmulqdq,-avx512vl,-uintr,-cf,+clflushopt,-raoint,-cmpccxadd,+bmi,-amx-tile,+sse,-gfni,-avxvnniint16,-amx-fp16,-ndd,+xsaveopt,+rdrnd,-avx512f,-amx-bf16,-avx512bf16,-avx512vnni,-push2pop2,+cx8,-avx512bw,+sse3,+pku,+fsgsbase,+clzero,+mwaitx,-lwp,+lzcnt,+sha,-movdir64b,-ppx,+wbnoinvd,-enqcmd,-avxneconvert,-tbm,-pconfig,-amx-complex,+ssse3,+cx16,+bmi2,+fma,+popcnt,-avxifma,+f16c,-avx512bitalg,+rdpru,+clwb,+mmx,+sse2,+rdseed,-avx512vbmi2,-prefetchi,+rdpid,-fma4,-avx512vbmi,+shstk,+vaes,-waitpkg,-sgx,+fxsr,-avx512dq,+sse4a," "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) "target-cpu"="znver3" "target-features"="+prfchw,-cldemote,+avx,+aes,+sahf,+pclmul,-xop,+crc32,+xsaves,-avx512fp16,-usermsr,-sm4,-egpr,+sse4.1,-avx512ifma,+xsave,+sse4.2,-tsxldtrk,-sm3,-ptwrite,-widekl,+invpcid,+64bit,+xsavec,-avx10.1-512,-avx512vpopcntdq,+cmov,-avx512vp2intersect,-avx512cd,+movbe,-avxvnniint8,-ccmp,-amx-int8,-kl,-avx10.1-256,-sha512,-avxvnni,-rtm,+adx,+avx2,-hreset,-movdiri,-serialize,+vpclmulqdq,-avx512vl,-uintr,-cf,+clflushopt,-raoint,-cmpccxadd,+bmi,-amx-tile,+sse,-gfni,-avxvnniint16,-amx-fp16,-ndd,+xsaveopt,+rdrnd,-avx512f,-amx-bf16,-avx512bf16,-avx512vnni,-push2pop2,+cx8,-avx512bw,+sse3,+pku,+fsgsbase,+clzero,+mwaitx,-lwp,+lzcnt,+sha,-movdir64b,-ppx,+wbnoinvd,-enqcmd,-avxneconvert,-tbm,-pconfig,-amx-complex,+ssse3,+cx16,+bmi2,+fma,+popcnt,-avxifma,+f16c,-avx512bitalg,+rdpru,+clwb,+mmx,+sse2,+rdseed,-avx512vbmi2,-prefetchi,+rdpid,-fma4,-avx512vbmi,+shstk,+vaes,-waitpkg,-sgx,+fxsr,-avx512dq,+sse4a," }
attributes #2 = { allocsize(0,1) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="znver3" "target-features"="+prfchw,-cldemote,+avx,+aes,+sahf,+pclmul,-xop,+crc32,+xsaves,-avx512fp16,-usermsr,-sm4,-egpr,+sse4.1,-avx512ifma,+xsave,+sse4.2,-tsxldtrk,-sm3,-ptwrite,-widekl,+invpcid,+64bit,+xsavec,-avx10.1-512,-avx512vpopcntdq,+cmov,-avx512vp2intersect,-avx512cd,+movbe,-avxvnniint8,-ccmp,-amx-int8,-kl,-avx10.1-256,-sha512,-avxvnni,-rtm,+adx,+avx2,-hreset,-movdiri,-serialize,+vpclmulqdq,-avx512vl,-uintr,-cf,+clflushopt,-raoint,-cmpccxadd,+bmi,-amx-tile,+sse,-gfni,-avxvnniint16,-amx-fp16,-ndd,+xsaveopt,+rdrnd,-avx512f,-amx-bf16,-avx512bf16,-avx512vnni,-push2pop2,+cx8,-avx512bw,+sse3,+pku,+fsgsbase,+clzero,+mwaitx,-lwp,+lzcnt,+sha,-movdir64b,-ppx,+wbnoinvd,-enqcmd,-avxneconvert,-tbm,-pconfig,-amx-complex,+ssse3,+cx16,+bmi2,+fma,+popcnt,-avxifma,+f16c,-avx512bitalg,+rdpru,+clwb,+mmx,+sse2,+rdseed,-avx512vbmi2,-prefetchi,+rdpid,-fma4,-avx512vbmi,+shstk,+vaes,-waitpkg,-sgx,+fxsr,-avx512dq,+sse4a," "tune-cpu"="generic" }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="znver3" "target-features"="+prfchw,-cldemote,+avx,+aes,+sahf,+pclmul,-xop,+crc32,+xsaves,-avx512fp16,-usermsr,-sm4,-egpr,+sse4.1,-avx512ifma,+xsave,+sse4.2,-tsxldtrk,-sm3,-ptwrite,-widekl,+invpcid,+64bit,+xsavec,-avx10.1-512,-avx512vpopcntdq,+cmov,-avx512vp2intersect,-avx512cd,+movbe,-avxvnniint8,-ccmp,-amx-int8,-kl,-avx10.1-256,-sha512,-avxvnni,-rtm,+adx,+avx2,-hreset,-movdiri,-serialize,+vpclmulqdq,-avx512vl,-uintr,-cf,+clflushopt,-raoint,-cmpccxadd,+bmi,-amx-tile,+sse,-gfni,-avxvnniint16,-amx-fp16,-ndd,+xsaveopt,+rdrnd,-avx512f,-amx-bf16,-avx512bf16,-avx512vnni,-push2pop2,+cx8,-avx512bw,+sse3,+pku,+fsgsbase,+clzero,+mwaitx,-lwp,+lzcnt,+sha,-movdir64b,-ppx,+wbnoinvd,-enqcmd,-avxneconvert,-tbm,-pconfig,-amx-complex,+ssse3,+cx16,+bmi2,+fma,+popcnt,-avxifma,+f16c,-avx512bitalg,+rdpru,+clwb,+mmx,+sse2,+rdseed,-avx512vbmi2,-prefetchi,+rdpid,-fma4,-avx512vbmi,+shstk,+vaes,-waitpkg,-sgx,+fxsr,-avx512dq,+sse4a," "tune-cpu"="generic" }
attributes #4 = { noreturn nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="znver3" "target-features"="+prfchw,-cldemote,+avx,+aes,+sahf,+pclmul,-xop,+crc32,+xsaves,-avx512fp16,-usermsr,-sm4,-egpr,+sse4.1,-avx512ifma,+xsave,+sse4.2,-tsxldtrk,-sm3,-ptwrite,-widekl,+invpcid,+64bit,+xsavec,-avx10.1-512,-avx512vpopcntdq,+cmov,-avx512vp2intersect,-avx512cd,+movbe,-avxvnniint8,-ccmp,-amx-int8,-kl,-avx10.1-256,-sha512,-avxvnni,-rtm,+adx,+avx2,-hreset,-movdiri,-serialize,+vpclmulqdq,-avx512vl,-uintr,-cf,+clflushopt,-raoint,-cmpccxadd,+bmi,-amx-tile,+sse,-gfni,-avxvnniint16,-amx-fp16,-ndd,+xsaveopt,+rdrnd,-avx512f,-amx-bf16,-avx512bf16,-avx512vnni,-push2pop2,+cx8,-avx512bw,+sse3,+pku,+fsgsbase,+clzero,+mwaitx,-lwp,+lzcnt,+sha,-movdir64b,-ppx,+wbnoinvd,-enqcmd,-avxneconvert,-tbm,-pconfig,-amx-complex,+ssse3,+cx16,+bmi2,+fma,+popcnt,-avxifma,+f16c,-avx512bitalg,+rdpru,+clwb,+mmx,+sse2,+rdseed,-avx512vbmi2,-prefetchi,+rdpid,-fma4,-avx512vbmi,+shstk,+vaes,-waitpkg,-sgx,+fxsr,-avx512dq,+sse4a," "tune-cpu"="generic" }
attributes #5 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #6 = { nounwind allocsize(0,1) }
attributes #7 = { nounwind }
attributes #8 = { noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 19.1.2 (++20241028122730+d8752671e825-1~exp1~20241028122742.57)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"long", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !7, i64 0}
!11 = distinct !{!11, !12, !13}
!12 = !{!"llvm.loop.mustprogress"}
!13 = !{!"llvm.loop.unroll.disable"}
!14 = !{!15, !15, i64 0}
!15 = !{!"any pointer", !7, i64 0}
!16 = distinct !{!16, !12, !13, !17, !18}
!17 = !{!"llvm.loop.isvectorized", i32 1}
!18 = !{!"llvm.loop.unroll.runtime.disable"}
!19 = distinct !{!19, !12, !13, !17}
