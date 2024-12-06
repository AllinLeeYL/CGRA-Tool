; ModuleID = 'kernel.ll'
source_filename = "kernel.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

@input = external local_unnamed_addr global [32 x float], align 4
@coefficients = external local_unnamed_addr global [32 x float], align 4

; Function Attrs: mustprogress nofree norecurse nounwind ssp uwtable(sync)
define dso_local void @main.extracted(float %0, ptr %.out) #0 {
newFuncRoot:
  br label %1

1:                                                ; preds = %newFuncRoot, %1
  %2 = phi i64 [ 0, %newFuncRoot ], [ %10, %1 ]
  %3 = phi float [ %0, %newFuncRoot ], [ %9, %1 ]
  %4 = getelementptr inbounds float, ptr @input, i64 %2
  %5 = load float, ptr %4, align 4, !tbaa !5
  %6 = getelementptr inbounds float, ptr @coefficients, i64 %2
  %7 = load float, ptr %6, align 4, !tbaa !5
  %8 = fmul float %5, %7
  %9 = fadd float %3, %8
  store float %9, ptr %.out, align 4
  %10 = add nuw nsw i64 %2, 1
  %11 = icmp eq i64 %10, 32
  br i1 %11, label %.exitStub, label %1, !llvm.loop !9

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
!4 = !{!"Homebrew clang version 19.1.0"}
!5 = !{!6, !6, i64 0}
!6 = !{!"float", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
