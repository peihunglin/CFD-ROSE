      program testcode
      dimension d(10)
      dimension e(10,10)
      dimension f(10,10,10)
!$pragma inline
      call test()
      end program
      subroutine test()
      dimension d(10)
      dimension e(10,10)
      dimension f(10,10,10)
      d(1) = 1.0
      e(3,4) = d(1)
      f(i1,i2,i3) = e(i4,i5) 
      end subroutine
