      program testcode
       f = 1.0
!$pragma inline
      call test(f)
!$pragma inline
      call test(f+1.5)
      end program
      subroutine test(f)
       d = f
      end subroutine
