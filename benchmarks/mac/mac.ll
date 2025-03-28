; ModuleID = 'mac.c'
source_filename = "mac.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@N = dso_local local_unnamed_addr global ptr null, align 8

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #0 {
  %1 = load ptr, ptr @N, align 8, !tbaa !5
  %2 = load volatile i32, ptr %1, align 4, !tbaa !9
  %3 = icmp sgt i32 %2, 2
  br i1 %3, label %4, label %18

4:                                                ; preds = %0
  %5 = add nsw i32 %2, -1
  %6 = zext i32 %5 to i64
  br label %7

7:                                                ; preds = %4, %7
  %8 = phi i64 [ 1, %4 ], [ %16, %7 ]
  %9 = phi i32 [ 0, %4 ], [ %15, %7 ]
  %10 = getelementptr inbounds i32, ptr inttoptr (i64 4 to ptr), i64 %8
  %11 = load i32, ptr %10, align 4, !tbaa !9
  %12 = getelementptr inbounds i32, ptr inttoptr (i64 36 to ptr), i64 %8
  %13 = load i32, ptr %12, align 4, !tbaa !9
  %14 = mul nsw i32 %13, %11
  %15 = add nsw i32 %14, %9
  %16 = add nuw nsw i64 %8, 1
  %17 = icmp eq i64 %16, %6
  br i1 %17, label %18, label %7, !llvm.loop !11

18:                                               ; preds = %7, %0
  %19 = phi i32 [ 0, %0 ], [ %15, %7 ]
  ret i32 %19
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
