; ModuleID = 'fibonacci.bc'
source_filename = "fibonacci.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [26 x i8] c"vector::_M_realloc_insert\00", align 1

; Function Attrs: mustprogress uwtable
define dso_local noundef i32 @_Z9fibonaccii(i32 noundef %0) local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %2 = icmp slt i32 %0, 0
  br i1 %2, label %74, label %3

3:                                                ; preds = %1
  %4 = tail call noalias noundef nonnull dereferenceable(4) ptr @_Znwm(i64 noundef 4) #7
  store i32 0, ptr %4, align 4, !tbaa !5
  %5 = invoke noalias noundef nonnull dereferenceable(8) ptr @_Znwm(i64 noundef 8) #7
          to label %6 unwind label %18

6:                                                ; preds = %3
  %7 = getelementptr inbounds i32, ptr %5, i64 1
  store i32 1, ptr %7, align 4, !tbaa !5
  store i32 0, ptr %5, align 4
  tail call void @_ZdlPv(ptr noundef nonnull %4) #8
  %8 = icmp ult i32 %0, 2
  br i1 %8, label %13, label %9

9:                                                ; preds = %6
  %10 = getelementptr inbounds i8, ptr %5, i64 8
  %11 = add nuw i32 %0, 1
  %12 = zext i32 %11 to i64
  br label %20

13:                                               ; preds = %60, %6
  %14 = phi ptr [ %5, %6 ], [ %63, %60 ]
  %15 = zext nneg i32 %0 to i64
  %16 = getelementptr inbounds i32, ptr %14, i64 %15
  %17 = load i32, ptr %16, align 4, !tbaa !5
  tail call void @_ZdlPv(ptr noundef nonnull %14) #8
  br label %74

18:                                               ; preds = %3
  %19 = landingpad { ptr, i32 }
          cleanup
  br label %71

20:                                               ; preds = %9, %60
  %21 = phi i64 [ 2, %9 ], [ %65, %60 ]
  %22 = phi ptr [ %5, %9 ], [ %63, %60 ]
  %23 = phi ptr [ %10, %9 ], [ %64, %60 ]
  %24 = phi ptr [ %10, %9 ], [ %61, %60 ]
  %25 = add nsw i64 %21, -1
  %26 = getelementptr inbounds i32, ptr %22, i64 %25
  %27 = load i32, ptr %26, align 4, !tbaa !5
  %28 = add nsw i64 %21, -2
  %29 = getelementptr inbounds i32, ptr %22, i64 %28
  %30 = load i32, ptr %29, align 4, !tbaa !5
  %31 = add nsw i32 %30, %27
  %32 = icmp eq ptr %23, %24
  br i1 %32, label %34, label %33

33:                                               ; preds = %20
  store i32 %31, ptr %23, align 4, !tbaa !5
  br label %60

34:                                               ; preds = %20
  %35 = ptrtoint ptr %23 to i64
  %36 = ptrtoint ptr %22 to i64
  %37 = sub i64 %35, %36
  %38 = icmp eq i64 %37, 9223372036854775804
  br i1 %38, label %39, label %41

39:                                               ; preds = %34
  invoke void @_ZSt20__throw_length_errorPKc(ptr noundef nonnull @.str) #9
          to label %40 unwind label %69

40:                                               ; preds = %39
  unreachable

41:                                               ; preds = %34
  %42 = ashr exact i64 %37, 2
  %43 = tail call i64 @llvm.umax.i64(i64 %42, i64 1)
  %44 = add nsw i64 %43, %42
  %45 = icmp ult i64 %44, %42
  %46 = tail call i64 @llvm.umin.i64(i64 %44, i64 2305843009213693951)
  %47 = select i1 %45, i64 2305843009213693951, i64 %46
  %48 = icmp eq i64 %47, 0
  br i1 %48, label %52, label %49

49:                                               ; preds = %41
  %50 = shl nuw nsw i64 %47, 2
  %51 = invoke noalias noundef nonnull ptr @_Znwm(i64 noundef %50) #7
          to label %52 unwind label %67

52:                                               ; preds = %49, %41
  %53 = phi ptr [ null, %41 ], [ %51, %49 ]
  %54 = getelementptr inbounds i32, ptr %53, i64 %42
  store i32 %31, ptr %54, align 4, !tbaa !5
  %55 = icmp sgt i64 %37, 0
  br i1 %55, label %56, label %57

56:                                               ; preds = %52
  tail call void @llvm.memmove.p0.p0.i64(ptr nonnull align 4 %53, ptr nonnull align 4 %22, i64 %37, i1 false)
  br label %57

57:                                               ; preds = %56, %52
  %58 = getelementptr inbounds i8, ptr %53, i64 %37
  tail call void @_ZdlPv(ptr noundef nonnull %22) #8
  %59 = getelementptr inbounds i32, ptr %53, i64 %47
  br label %60

60:                                               ; preds = %57, %33
  %61 = phi ptr [ %59, %57 ], [ %24, %33 ]
  %62 = phi ptr [ %58, %57 ], [ %23, %33 ]
  %63 = phi ptr [ %53, %57 ], [ %22, %33 ]
  %64 = getelementptr inbounds i32, ptr %62, i64 1
  %65 = add nuw nsw i64 %21, 1
  %66 = icmp eq i64 %65, %12
  br i1 %66, label %13, label %20, !llvm.loop !9

67:                                               ; preds = %49
  %68 = landingpad { ptr, i32 }
          cleanup
  br label %71

69:                                               ; preds = %39
  %70 = landingpad { ptr, i32 }
          cleanup
  br label %71

71:                                               ; preds = %67, %69, %18
  %72 = phi ptr [ %4, %18 ], [ %22, %67 ], [ %22, %69 ]
  %73 = phi { ptr, i32 } [ %19, %18 ], [ %68, %67 ], [ %70, %69 ]
  tail call void @_ZdlPv(ptr noundef nonnull %72) #8
  resume { ptr, i32 } %73

74:                                               ; preds = %1, %13
  %75 = phi i32 [ %17, %13 ], [ -1, %1 ]
  ret i32 %75
}

declare i32 @__gxx_personality_v0(...)

; Function Attrs: mustprogress norecurse uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
  %1 = tail call noundef i32 @_Z9fibonaccii(i32 noundef 10)
  ret i32 0
}

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(ptr noundef) local_unnamed_addr #2

; Function Attrs: noreturn
declare void @_ZSt20__throw_length_errorPKc(ptr noundef) local_unnamed_addr #3

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znwm(i64 noundef) local_unnamed_addr #4

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr nocapture writeonly, ptr nocapture readonly, i64, i1 immarg) #5

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #6

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umin.i64(i64, i64) #6

attributes #0 = { mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress norecurse uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nobuiltin nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nobuiltin allocsize(0) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #6 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #7 = { builtin allocsize(0) }
attributes #8 = { builtin nounwind }
attributes #9 = { noreturn }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
