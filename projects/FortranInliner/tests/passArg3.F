      program testcode
      dimension f(20)
!$pragma inline
      call test(f)
      end program
      subroutine test(f)
      dimension f(10)
      end subroutine
