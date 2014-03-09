      program testcode
      dimension f(4,4,4)
      f(1,1,1) = 0.0
!$pragma inline
      call test(f)
      f(1,1,1) = 5.0
!$pragma inline
      call test2(f(1,1,3))
      end program
c
      subroutine test(f)
      dimension f(64)
      f(1) = 1.0
      end subroutine
c
      subroutine test2(f)
      dimension f(16)
      f(1) = 1.0
      end subroutine
