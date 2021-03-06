!<license>
!    Copyright (C) 2017 State of California,
!    Department of Water Resources.
!    This file is part of DSM2-GTM.
!
!    The Delta Simulation Model 2 (DSM2) - General Transport Model (GTM) 
!    is free software: you can redistribute it and/or modify
!    it under the terms of the GNU General Public License as published by
!    the Free Software Foundation, either version 3 of the License, or
!    (at your option) any later version.
!
!    DSM2 is distributed in the hope that it will be useful,
!    but WITHOUT ANY WARRANTY; without even the implied warranty of
!    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!    GNU General Public License for more details.
!
!    You should have received a copy of the GNU General Public License
!    along with DSM2.  If not, see <http://www.gnu.org/licenses>.
!</license>

module create_synth_tide

    ! 5000ft-->1524m, domain_length=1524*20=30480m
    ! nx_base=20
    ! to create one-year length data, total_time=424*2*m2_period
    ! nstep_base= 424*24*4=40704
    
    use gtm_precision
    
    implicit none
    real(gtm_real), parameter :: m2ft = 3.28084d0
    integer, parameter  :: nconc = 1                              !< Number of constituents
    integer, parameter  :: nstep_base = 40704                     !< Number of time steps in finer discritization
    integer, parameter  :: nx_base = 20                           !< Number of spatial discritization in finer mesh 
    real(gtm_real),parameter :: origin = zero                     !< Left hand side of the channel
    real(gtm_real),parameter :: domain_length = 5000.0d0/m2ft*20.d0             !< Domain Length in meter
    real(gtm_real),parameter :: amplitude = half                  !< Tidal amplitude in meter    
    real(gtm_real),parameter :: gravity = 9.80d0                  !< Gravitational acceleration in m/s^2
    real(gtm_real),parameter :: depth = 16.0d0                    !< Channel depth in meter
    real(gtm_real),parameter :: sec_per_hr = 60.d0*60.d0          !< Convert factor of hour to second 
    real(gtm_real),parameter :: m2_period = 12.4d0*sec_per_hr     !< rough tidal period (shouold be 12.4)
    real(gtm_real),parameter :: freq=two*pi/m2_period             !< Frequency of tidal oscillation
    real(gtm_real),parameter :: total_time = m2_period*two*424.d0      !< Total time of the test
    real(gtm_real),parameter :: start_time = zero                 !< Starts at zero

    contains

    subroutine print_tidal_to_file()
        implicit none
        real(gtm_real) :: dt
        real(gtm_real) :: time
        real(gtm_real) :: dx(nx_base)        
        real(gtm_real) :: flow_lo(nx_base)
        real(gtm_real) :: flow_hi(nx_base)
        real(gtm_real) :: area_lo(nx_base)
        real(gtm_real) :: area_hi(nx_base)
        integer :: i, j
        dx = domain_length/dble(nx_base)
        dt = total_time/nstep_base
        open(1001,file="tidal_flow_gen.txt")
        open(1002,file="tidal_ws_gen.txt")
        do i = 1, nstep_base
            time = start_time + dt*dble(i-1)
            call tidal_flow_generator(flow_lo, flow_hi, area_lo, area_hi, nx_base, time, dx, dt)
            ! width=1m, unit conversion: 1m3/s=35.3146667ft3/s, 1m=3.28084ft
            write(1001,'(80f30.24)') (flow_lo(j)*m2ft**3, flow_hi(j)*m2ft**3, j = 1, nx_base)
            write(1002,'(80f30.24)') (area_lo(j)*m2ft+0.1d0, area_hi(j)*m2ft+0.1d0, j = 1, nx_base) !+0.1 is to fix hydro having problem in giving 0 elevation
        end do
        close(1001)
        close(1002)
        return
    end subroutine

    !> Tidal flow for a rectangular basin with periodic forcing in the cell centerd 
    !> the area here retrived from dQ/dx+dA/dt=0 --> A =width*(depth+int(-dQ/dx,t)) 
    subroutine tidal_flow_generator(flow_lo,  &
                                    flow_hi,  &
                                    area_lo,  &
                                    area_hi,  &
                                    ncell,    &
                                    time,     &
                                    dx,       &
                                    dt)                     
        implicit none
        integer, intent(in) :: ncell                   !< number of cells
        real(gtm_real), intent(in)  :: time            !< time of request
        real(gtm_real), intent(in)  :: dx(ncell)       !< spatial step 
        real(gtm_real), intent(in)  :: dt              !< time step 
        real(gtm_real), intent(out) :: flow_lo(ncell)  !< lo face flow
        real(gtm_real), intent(out) :: flow_hi(ncell)  !< hi face flow
        real(gtm_real), intent(out) :: area_lo(ncell)  !< area lo face
        real(gtm_real), intent(out) :: area_hi(ncell)  !< area hi face
        real(gtm_real) :: flow(ncell)                  !< cell centered flow
        real(gtm_real) :: area(ncell)                  !< cell center area

        !--- local
        real(gtm_real) :: half_time
        real(gtm_real) :: old_time
        real(gtm_real) :: big_b 
        real(gtm_real) :: big_a 
        real(gtm_real) :: vel_lo
        real(gtm_real) :: vel_hi
        real(gtm_real) :: vel
        real(gtm_real) :: xpos_lo
        real(gtm_real) :: xpos_hi
        real(gtm_real) :: xpos
        real(gtm_real) :: flow_term1
        real(gtm_real) :: flow_term2
        integer :: icell

        half_time = time - half*dt
        old_time = time - dt
        big_b = freq/dsqrt(gravity*depth)
        big_a = amplitude* dsqrt(gravity*depth)/(depth*dcos(big_b*domain_length))

        ! width is assumed to be equal to 1 meter 
        do icell = 1,ncell  
  
        ! this is L-x
        xpos_lo = domain_length- dble(icell-1)*dx(icell)
        xpos_hi = domain_length- dble(icell)*dx(icell)
        xpos    = domain_length-(dble(icell)-half)*dx(icell)
  
        area(icell) = (big_a/(freq*dx(icell)))*(depth*dcos(freq*time)*(dsin(big_b*xpos_hi)-dsin(big_b*xpos_lo))+&
                       amplitude*dcos(two*freq*time)*(dsin(two*big_b*xpos_hi)-dsin(two*big_b*xpos_lo))/(eight*dcos(big_b*domain_length)))
        ! todo: is it a correction factor for constant in integeration?
        ! check this
        area(icell) = depth + area(icell)
        area_lo(icell) = depth + amplitude * dcos(big_b*xpos_lo)/dcos(big_b*domain_length)*dcos(freq*half_time)  

        area_lo(icell) = big_a*big_b*(-depth*dcos(big_b*xpos_lo)*dcos(freq*half_time)/freq - &
                         amplitude*dcos(two*freq*half_time)*dcos(two*big_b*xpos_lo)&
                         /(four*freq*dcos(big_b*domain_length)))
        ! todo:
        ! i just match them base on old values ???                             
        area_lo(icell) = depth + area_lo(icell)
 
        area_hi(icell) = depth + amplitude * dcos(big_b*xpos_hi)/dcos(big_b*domain_length)*dcos(freq*half_time) 
   
        area_hi(icell) = big_a*big_b*(-depth*dcos(big_b*xpos_hi)*dcos(freq*half_time)/freq - &
                         amplitude*dcos(two*freq*half_time)*dcos(two*big_b*xpos_hi)&
                         /(four*freq*dcos(big_b*domain_length)))
   
        area_hi(icell) = depth + area_hi(icell)
                         
        flow(icell)    = big_a*depth*dsin(freq*time)*(dcos(big_b*xpos_hi)-dcos(big_b*xpos_lo))/(dx(icell)*big_b) &
                         + big_a*amplitude*dsin(two*freq*time)*(one/(four*dx(icell)*big_b*dcos(big_b*domain_length))) * &
                        (dcos(big_b*xpos_hi)**two - dcos(big_b*xpos_lo)**two)
  
        flow_term1 = - big_a*depth*dsin(big_b*xpos_lo)*(dcos(freq*time)-dcos(freq*old_time))/(dt*freq)
        flow_term2 = - big_a*amplitude*dsin(two*big_b*xpos_lo)*(dcos(freq*time)**two-dcos(freq*old_time)**two)&
                     /(four*freq*dcos(big_b*domain_length)*dt)
        flow_lo(icell) = flow_term1+flow_term2
          
        flow_term1 = - big_a*depth*dsin(big_b*xpos_hi)*(dcos(freq*time)-dcos(freq*old_time))/(dt*freq)
        flow_term2 = - big_a*amplitude*dsin(two*big_b*xpos_hi)*(dcos(freq*time)**two-dcos(freq*old_time)**two)&
                     /(four*freq*dcos(big_b*domain_length)*dt)
        flow_hi(icell) = flow_term1+flow_term2
        end do  
        return
    end subroutine
end module