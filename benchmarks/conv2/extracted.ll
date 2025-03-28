; ModuleID = 'conv2.ll'
source_filename = "conv2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local void @extracted(i64 %0) #0 {
newFuncRoot:
  br label %1

1:                                                ; preds = %newFuncRoot, %1
  %2 = phi i64 [ 1, %newFuncRoot ], [ %6, %1 ]
  %3 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %2
  %4 = load i32, ptr %3, align 4, !tbaa !5
  %5 = mul nsw i32 %4, 10
  %6 = add nuw nsw i64 %2, 1
  %7 = getelementptr inbounds i32, ptr inttoptr (i64 2560 to ptr), i64 %6
  %8 = load i32, ptr %7, align 4, !tbaa !5
  %9 = mul nsw i32 %8, 20
  %10 = add nsw i32 %9, %5
  %11 = getelementptr inbounds i32, ptr inttoptr (i64 2816 to ptr), i64 %2
  store i32 %10, ptr %11, align 4, !tbaa !5
  %12 = icmp eq i64 %6, %0
  br i1 %12, label %.exitStub, label %1, !llvm.loop !9

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
