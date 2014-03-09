      program testcode
      dimension f(20)
      i = i + 1
!$pragma inline
      call test(f)
      i = i + 2
      end program
c
      subroutine test(f)
      dimension f(4,5)
      f(1,1) = 1.0
      end subroutine
