; ModuleID = 'matrix-multiply.bc'
source_filename = "matrix-multiply.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@resultAB1 = dso_local global [20 x [20 x i32]] zeroinitializer, align 16
@.str = private unnamed_addr constant [14 x i8] c"Result: %lld\0A\00", align 1
@str = private unnamed_addr constant [13 x i8] c"RESULT: FAIL\00", align 1
@str.3 = private unnamed_addr constant [13 x i8] c"RESULT: PASS\00", align 1

; Function Attrs: nofree noinline norecurse nounwind memory(readwrite, argmem: read) uwtable
define dso_local i32 @multiply(i32 noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = sext i32 %0 to i64
  %4 = sext i32 %1 to i64
  br label %5

5:                                                ; preds = %2, %5
  %6 = phi i64 [ 0, %2 ], [ %14, %5 ]
  %7 = phi i32 [ 0, %2 ], [ %13, %5 ]
  %8 = getelementptr inbounds [20 x i32], ptr inttoptr (i64 2560 to ptr), i64 %3, i64 %6
  %9 = load i32, ptr %8, align 4, !tbaa !5
  %10 = getelementptr inbounds [20 x i32], ptr inttoptr (i64 2816 to ptr), i64 %6, i64 %4
  %11 = load i32, ptr %10, align 4, !tbaa !5
  %12 = mul nsw i32 %11, %9
  %13 = add nsw i32 %12, %7
  %14 = add nuw nsw i64 %6, 1
  %15 = icmp eq i64 %14, 20
  br i1 %15, label %16, label %5, !llvm.loop !9

16:                                               ; preds = %5
  %17 = sext i32 %0 to i64
  %18 = sext i32 %1 to i64
  %19 = getelementptr inbounds [20 x [20 x i32]], ptr @resultAB1, i64 0, i64 %17, i64 %18
  store volatile i32 %13, ptr %19, align 4, !tbaa !5
  ret i32 %13
}

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #1 {
  br label %1

1:                                                ; preds = %0, %12
  %2 = phi i64 [ 0, %0 ], [ %9, %12 ]
  %3 = phi i32 [ 0, %0 ], [ %13, %12 ]
  br label %4

4:                                                ; preds = %1, %4
  %5 = phi i64 [ %2, %1 ], [ %9, %4 ]
  %6 = phi i32 [ 0, %1 ], [ %10, %4 ]
  %7 = tail call i32 @multiply(i32 noundef %3, i32 noundef %6)
  %8 = sext i32 %7 to i64
  %9 = add i64 %5, %8
  %10 = add nuw nsw i32 %6, 1
  %11 = icmp eq i32 %10, 20
  br i1 %11, label %12, label %4, !llvm.loop !12

12:                                               ; preds = %4
  %13 = add nuw nsw i32 %3, 1
  %14 = icmp eq i32 %13, 20
  br i1 %14, label %15, label %1, !llvm.loop !13

15:                                               ; preds = %12
  %16 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i64 noundef %9)
  %17 = icmp eq i64 %9, 962122000
  %18 = select i1 %17, ptr @str.3, ptr @str
  %19 = tail call i32 @puts(ptr nonnull dereferenceable(1) %18)
  %20 = trunc i64 %9 to i32
  ret i32 %20
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #3

attributes #0 = { nofree noinline norecurse nounwind memory(readwrite, argmem: read) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }

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
!12 = distinct !{!12, !10, !11}
!13 = distinct !{!13, !10, !11}
