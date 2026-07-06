; ModuleID = 'dsp.bc'
source_filename = "dsp.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(argmem: readwrite) uwtable
define dso_local void @_Z14process_signalPKiPi(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1) local_unnamed_addr #0 {
  br label %4

3:                                                ; preds = %4
  ret void

4:                                                ; preds = %2, %4
  %5 = phi i64 [ 0, %2 ], [ %12, %4 ]
  %6 = phi i32 [ 0, %2 ], [ %10, %4 ]
  %7 = getelementptr inbounds i32, ptr %0, i64 %5
  %8 = load i32, ptr %7, align 4, !tbaa !5
  %9 = mul nsw i32 %8, 3
  %10 = add nsw i32 %9, %6
  %11 = getelementptr inbounds i32, ptr %1, i64 %5
  store i32 %10, ptr %11, align 4, !tbaa !5
  %12 = add nuw nsw i64 %5, 1
  %13 = icmp eq i64 %12, 16
  br i1 %13, label %3, label %4, !llvm.loop !9
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind memory(argmem: readwrite) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
