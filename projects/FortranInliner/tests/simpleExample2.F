      program testcode
      f = 2.0
!$pragma inline
      call test()
      end program
      subroutine test()
        f = f + 1.0
      end subroutine
