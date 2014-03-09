      program testcode
      dimension f(20)
      f(1) = 1.0
!$pragma inline
      call test(f(11))
      end program
      subroutine test(f)
      dimension f(10)
      f(3) = 5.0
      end subroutine
