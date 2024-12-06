; ModuleID = 'sum.ll'
source_filename = "sum.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: mustprogress nofree norecurse nounwind ssp uwtable(sync)
define dso_local void @_Z6kernelv.extracted(i64 %0, ptr %.out) #0 {
newFuncRoot:
  br label %1

1:                                                ; preds = %newFuncRoot, %1
  %2 = phi i64 [ 0, %newFuncRoot ], [ %7, %1 ]
  %3 = phi i32 [ 0, %newFuncRoot ], [ %6, %1 ]
  %4 = getelementptr inbounds i32, ptr inttoptr (i64 4 to ptr), i64 %2
  %5 = load i32, ptr %4, align 4, !tbaa !5
  %6 = add nsw i32 %5, %3
  store i32 %6, ptr %.out, align 4
  %7 = add nuw nsw i64 %2, 1
  %8 = icmp eq i64 %7, %0
  br i1 %8, label %.exitStub, label %1, !llvm.loop !9

.exitStub:                                        ; preds = %1
  ret void
}

attributes #0 = { mustprogress nofree norecurse nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 19.1.2"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
