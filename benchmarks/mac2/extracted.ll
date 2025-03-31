; ModuleID = 'mac2.ll'
source_filename = "mac2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local void @main.extracted(i64 %0, ptr %.out, ptr %.out1) #0 {
newFuncRoot:
  br label %1

1:                                                ; preds = %newFuncRoot, %1
  %2 = phi i64 [ 1, %newFuncRoot ], [ %20, %1 ]
  %3 = phi i32 [ 0, %newFuncRoot ], [ %19, %1 ]
  %4 = phi i32 [ 0, %newFuncRoot ], [ %10, %1 ]
  %5 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %2
  %6 = load i32, ptr %5, align 4, !tbaa !5
  %7 = getelementptr inbounds i32, ptr inttoptr (i64 2816 to ptr), i64 %2
  %8 = load i32, ptr %7, align 4, !tbaa !5
  %9 = mul nsw i32 %8, %6
  %10 = add nsw i32 %9, %4
  store i32 %10, ptr %.out, align 4
  %11 = add nsw i32 %8, 1
  %12 = mul nsw i32 %11, %6
  %13 = getelementptr inbounds i32, ptr inttoptr (i64 3072 to ptr), i64 %2
  %14 = load i32, ptr %13, align 4, !tbaa !5
  %15 = mul nsw i32 %12, %14
  %16 = getelementptr inbounds i32, ptr inttoptr (i64 3328 to ptr), i64 %2
  %17 = load i32, ptr %16, align 4, !tbaa !5
  %18 = mul nsw i32 %15, %17
  %19 = add nsw i32 %18, %3
  store i32 %19, ptr %.out1, align 4
  %20 = add nuw nsw i64 %2, 1
  %21 = icmp eq i64 %20, %0
  br i1 %21, label %.exitStub, label %1, !llvm.loop !9

.exitStub:                                        ; preds = %1
  ret void
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
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
