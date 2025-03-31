; ModuleID = 'mac2.c'
source_filename = "mac2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@N = dso_local local_unnamed_addr global ptr inttoptr (i64 3840 to ptr), align 8

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #0 {
  %1 = load ptr, ptr @N, align 8, !tbaa !5
  %2 = load volatile i32, ptr %1, align 4, !tbaa !9
  %3 = icmp sgt i32 %2, 2
  br i1 %3, label %4, label %30

4:                                                ; preds = %0
  %5 = add nsw i32 %2, -1
  %6 = zext i32 %5 to i64
  br label %7

7:                                                ; preds = %4, %7
  %8 = phi i64 [ 1, %4 ], [ %26, %7 ]
  %9 = phi i32 [ 0, %4 ], [ %25, %7 ]
  %10 = phi i32 [ 0, %4 ], [ %16, %7 ]
  %11 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %8
  %12 = load i32, ptr %11, align 4, !tbaa !9
  %13 = getelementptr inbounds i32, ptr inttoptr (i64 2816 to ptr), i64 %8
  %14 = load i32, ptr %13, align 4, !tbaa !9
  %15 = mul nsw i32 %14, %12
  %16 = add nsw i32 %15, %10
  %17 = add nsw i32 %14, 1
  %18 = mul nsw i32 %17, %12
  %19 = getelementptr inbounds i32, ptr inttoptr (i64 3072 to ptr), i64 %8
  %20 = load i32, ptr %19, align 4, !tbaa !9
  %21 = mul nsw i32 %18, %20
  %22 = getelementptr inbounds i32, ptr inttoptr (i64 3328 to ptr), i64 %8
  %23 = load i32, ptr %22, align 4, !tbaa !9
  %24 = mul nsw i32 %21, %23
  %25 = add nsw i32 %24, %9
  %26 = add nuw nsw i64 %8, 1
  %27 = icmp eq i64 %26, %6
  br i1 %27, label %28, label %7, !llvm.loop !11

28:                                               ; preds = %7
  %29 = add nsw i32 %25, %16
  br label %30

30:                                               ; preds = %28, %0
  %31 = phi i32 [ 0, %0 ], [ %29, %28 ]
  ret i32 %31
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
