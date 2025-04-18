
program loops 

  use omp_lib 

  implicit none 
  integer, parameter :: N=2183
  integer, parameter :: reps=1000 
  
  real(kind=8), allocatable ::  xx(:,:), yy(:,:) 
  real(kind=8) :: zz 
  integer :: lenj(N)  

  
  real(kind=8) :: start1,start2,end1,end2
  integer :: r

  allocate(xx(N,N), yy(N,N)) 

  call init1()  

  start1 = omp_get_wtime()
 
  do r = 1,reps
     call loop1() 
  end do

  end1  = omp_get_wtime()  

  call valid1(); 

  print *, "Total time for ",reps," reps of loop 1 = ", end1-start1 

  call init2()  

  start2 = omp_get_wtime()
 
  do r = 1,reps
     call loop2() 
  end do

  end2  = omp_get_wtime()  

  call valid2(); 

  print *, "Total time for ",reps," reps of loop 2 = ", end2-start2 


contains 

subroutine init1()

  implicit none 

  integer ::  i,j
 
  do i = 1,N 
     do j = 1,N 
        xx(j,i) = 0.0 
        yy(j,i) = 1.618*(i+j)
     end do
  end do

end subroutine init1 


subroutine init2()

  implicit none 

  integer ::  i,j,expr

  do i = 1,N 
     expr = mod(i,4*(i/70)+1)
     if (expr == 0) then
        lenj(i) = N/3 
     else
        lenj(i) = 3
     end if
  end do

  zz = 0.0 

  do i = 1,N 
     do j = 1,N 
        yy(j,i) = dble(i*j+1)/dble(N*N)
     end do
  end do

end subroutine init2
 

subroutine loop1() 

  implicit none 

  integer ::  i,j
  
!$omp parallel do default(none)  shared(xx,yy) schedule(runtime)
  do i = 1,N
     do j = 1,i 
        xx(j,i) = xx(j,i) + log(yy(j,i)) / N 
     end do
  end do

end subroutine loop1 



subroutine loop2() 

  implicit none 

  integer :: i,j,k
  real (kind=8) :: rNN  

  rNN = 1.0 / dble (N*N)  

!$omp parallel do default(none) shared(yy,rNN,lenj) reduction(+:zz) schedule(runtime)
  do i = 1,N
     do j = 1, lenj(i) 
        do k = 1,j 
           zz = zz + k * yy(j,i) * yy(j,i) *rNN
        end do
     end do
  end do

end subroutine loop2



subroutine valid1()
 
  implicit none 

  integer :: i,j 
  real (kind=8) :: sumxx 
  
  sumxx= 0.0

  do i = 1,N 
     do j = 1,N 
        sumxx = sumxx + xx(j,i) 
     end do
  end do

  print *, "Loop 1 check: Sum of xx is ", sumxx

end subroutine valid1



subroutine valid2()
  
  implicit none 

  print *, "Loop 2 check: zz is ", zz

end subroutine valid2  
 

end program loops 
