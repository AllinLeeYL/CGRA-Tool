; ModuleID = 'kernel.bc'
source_filename = "kernel.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

@input = local_unnamed_addr global [32 x float] zeroinitializer, align 4
@output = local_unnamed_addr global [32 x float] zeroinitializer, align 4
@coefficients = local_unnamed_addr global [32 x float] [float 2.500000e-01, float 1.500000e+00, float 3.750000e+00, float -2.250000e+00, float 5.000000e-01, float 7.500000e-01, float -3.000000e+00, float 1.250000e+00, float 2.500000e-01, float 1.500000e+00, float 3.750000e+00, float -2.250000e+00, float 5.000000e-01, float 7.500000e-01, float -3.000000e+00, float 1.250000e+00, float 2.500000e-01, float 1.500000e+00, float 3.750000e+00, float -2.250000e+00, float 5.000000e-01, float 7.500000e-01, float -3.000000e+00, float 1.250000e+00, float 2.500000e-01, float 1.500000e+00, float 3.750000e+00, float -2.250000e+00, float 5.000000e-01, float 7.500000e-01, float -3.000000e+00, float 1.250000e+00], align 4

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp memory(readwrite, argmem: none, inaccessiblemem: none) uwtable(sync)
define noundef i32 @main() local_unnamed_addr #0 {
  %1 = load float, ptr @output, align 4, !tbaa !5
  br label %2

2:                                                ; preds = %2, %0
  %3 = phi i64 [ 0, %0 ], [ %11, %2 ]
  %4 = phi float [ %1, %0 ], [ %10, %2 ]
  %5 = getelementptr inbounds float, ptr @input, i64 %3
  %6 = load float, ptr %5, align 4, !tbaa !5
  %7 = getelementptr inbounds float, ptr @coefficients, i64 %3
  %8 = load float, ptr %7, align 4, !tbaa !5
  %9 = fmul float %6, %8
  %10 = fadd float %4, %9
  %11 = add nuw nsw i64 %3, 1
  %12 = icmp eq i64 %11, 32
  br i1 %12, label %13, label %2, !llvm.loop !9

13:                                               ; preds = %2
  %.lcssa = phi float [ %10, %2 ]
  store float %.lcssa, ptr @output, align 4, !tbaa !5
  ret i32 0
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp memory(argmem: readwrite) uwtable(sync)
define void @_Z6kernelPfS_S_(ptr nocapture noundef readonly %0, ptr nocapture noundef %1, ptr nocapture noundef readonly %2) local_unnamed_addr #1 {
  %4 = load float, ptr %1, align 4, !tbaa !5
  br label %5

5:                                                ; preds = %3, %5
  %6 = phi i64 [ 0, %3 ], [ %14, %5 ]
  %7 = phi float [ %4, %3 ], [ %13, %5 ]
  %8 = getelementptr inbounds float, ptr %0, i64 %6
  %9 = load float, ptr %8, align 4, !tbaa !5
  %10 = getelementptr inbounds float, ptr %2, i64 %6
  %11 = load float, ptr %10, align 4, !tbaa !5
  %12 = fmul float %9, %11
  %13 = fadd float %7, %12
  store float %13, ptr %1, align 4, !tbaa !5
  %14 = add nuw nsw i64 %6, 1
  %15 = icmp eq i64 %14, 32
  br i1 %15, label %16, label %5, !llvm.loop !9

16:                                               ; preds = %5
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind ssp memory(readwrite, argmem: none, inaccessiblemem: none) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind ssp memory(argmem: readwrite) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 19.1.0"}
!5 = !{!6, !6, i64 0}
!6 = !{!"float", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
