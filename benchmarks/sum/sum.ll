; ModuleID = 'sum.bc'
source_filename = "sum.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

@n = local_unnamed_addr global ptr null, align 8

; Function Attrs: mustprogress nofree norecurse nounwind ssp uwtable(sync)
define noundef i32 @_Z6kernelv() local_unnamed_addr #0 {
  %1 = load ptr, ptr @n, align 8, !tbaa !5
  %2 = load volatile i32, ptr %1, align 4, !tbaa !9
  %3 = icmp sgt i32 %2, 0
  br i1 %3, label %4, label %14

4:                                                ; preds = %0
  %5 = zext nneg i32 %2 to i64
  br label %6

6:                                                ; preds = %4, %6
  %7 = phi i64 [ 0, %4 ], [ %12, %6 ]
  %8 = phi i32 [ 0, %4 ], [ %11, %6 ]
  %9 = getelementptr inbounds i32, ptr inttoptr (i64 4 to ptr), i64 %7
  %10 = load i32, ptr %9, align 4, !tbaa !9
  %11 = add nsw i32 %10, %8
  %12 = add nuw nsw i64 %7, 1
  %13 = icmp eq i64 %12, %5
  br i1 %13, label %14, label %6, !llvm.loop !11

14:                                               ; preds = %6, %0
  %15 = phi i32 [ 0, %0 ], [ %11, %6 ]
  ret i32 %15
}

; Function Attrs: mustprogress nofree norecurse nounwind ssp willreturn uwtable(sync)
define noundef i32 @main() local_unnamed_addr #1 {
  %1 = load ptr, ptr @n, align 8, !tbaa !5
  %2 = load volatile i32, ptr %1, align 4, !tbaa !9
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }
attributes #1 = { mustprogress nofree norecurse nounwind ssp willreturn uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 19.1.2"}
!5 = !{!6, !6, i64 0}
!6 = !{!"any pointer", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !7, i64 0}
!11 = distinct !{!11, !12, !13}
!12 = !{!"llvm.loop.mustprogress"}
!13 = !{!"llvm.loop.unroll.disable"}
