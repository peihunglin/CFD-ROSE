      program testcode
      dimension f(10)
!$pragma inline
      call test(f(1))
      end program
      subroutine test(f)
      dimension f(10)
      end subroutine
