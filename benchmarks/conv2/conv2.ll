; ModuleID = 'conv2.bc'
source_filename = "conv2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@N = dso_local local_unnamed_addr global ptr inttoptr (i64 3840 to ptr), align 8

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #0 {
  %1 = load ptr, ptr @N, align 8, !tbaa !5
  %2 = load volatile i32, ptr %1, align 4, !tbaa !9
  %3 = icmp sgt i32 %2, 2
  br i1 %3, label %4, label %19

4:                                                ; preds = %0
  %5 = add nsw i32 %2, -1
  %6 = zext i32 %5 to i64
  br label %7

7:                                                ; preds = %4, %7
  %8 = phi i64 [ 1, %4 ], [ %12, %7 ]
  %9 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %8
  %10 = load i32, ptr %9, align 4, !tbaa !9
  %11 = mul nsw i32 %10, 10
  %12 = add nuw nsw i64 %8, 1
  %13 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %12
  %14 = load i32, ptr %13, align 4, !tbaa !9
  %15 = mul nsw i32 %14, 20
  %16 = add nsw i32 %15, %11
  %17 = getelementptr inbounds i32, ptr inttoptr (i64 2816 to ptr), i64 %8
  store i32 %16, ptr %17, align 4, !tbaa !9
  %18 = icmp eq i64 %12, %6
  br i1 %18, label %19, label %7, !llvm.loop !11

19:                                               ; preds = %7, %0
  ret i32 0
}

attributes #0 = { nofree norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 20.0.0git (git@github.com:AllinLeeYL/llvm-project.git 48736518c9b24f51ca02af02a7b469e230ea6bcc)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"any pointer", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !7, i64 0}
!11 = distinct !{!11, !12, !13}
!12 = !{!"llvm.loop.mustprogress"}
!13 = !{!"llvm.loop.unroll.disable"}
