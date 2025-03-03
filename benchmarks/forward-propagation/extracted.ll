; ModuleID = 'vectoradd.ll'
source_filename = "vectoradd.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: mustprogress nofree norecurse nounwind ssp uwtable(sync)
define dso_local void @extracted(float %0, ptr %1, ptr %2, ptr %3) #0 {
newFuncRoot:
  br label %4

4:                                                ; preds = %newFuncRoot, %4
  %5 = phi i64 [ 0, %newFuncRoot ], [ %13, %4 ]
  %6 = phi float [ %0, %newFuncRoot ], [ %12, %4 ]
  %7 = getelementptr inbounds float, ptr %1, i64 %5
  %8 = load float, ptr %7, align 4, !tbaa !5
  %9 = getelementptr inbounds float, ptr %2, i64 %5
  %10 = load float, ptr %9, align 4, !tbaa !5
  %11 = fmul float %8, %10
  %12 = fadd float %6, %11
  store float %12, ptr %3, align 4, !tbaa !5
  %13 = add nuw nsw i64 %5, 1
  %14 = icmp eq i64 %13, 32
  br i1 %14, label %.exitStub, label %4, !llvm.loop !9

.exitStub:                                        ; preds = %4
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
!6 = !{!"float", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
