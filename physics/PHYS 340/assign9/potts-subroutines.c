/*
This assignment consists of writing three subroutines that will be used
in the implementation of the code for the simulation of the
2-dimensional Potts model.  Mathematically, the Potts model is described
by the energy function

	E = J*(Sum over nearest neighbor spins i & j) (1.-delta(si,sj))

The constant J sets the energy scale, and we take J=1 without loss of
generality.  The spins can take any integer value in the range 0->q-1.
The function delta(si,sj) is the Kronecker delta function, which is 1 if
si=sj, and zero otherwise.  Clearly E is minimized when the spins are in
the same orientation, so low temperatures will favor the spins having
the same orientations.

Since we must have a finite lattice in our simulation code, a question
arises as to what we should do at the boundaries.  To minimize the
effects of the boundaries, we will use periodic boundary conditions.
That is, the last spin in the x-direction is linked with the zeroth spin
in the x-direction, and similar links in the y-direction.  Therefore if
we have a lattice of size Nx by Ny, the spins [0][y] and [Nx-1][y] are
all nearest neighbors, as are spins [x][0] and [x][Ny-1].

When linked with the control code, these subroutines perform a certain
number of Monte Carlo (MC) iterations with the Metropolis method and
makes occasional measurements of the energy and magnetization.  At the
end, the spin configuration is saved on a spins.dat file, which can be
used to continue the simulation in a subsequent run.  The results of the
measurements are written on a results.dat file.  This is a temporary
file, which will be overwritten the next time the code is run.  It is
the responsibility of the person running the code to transfer the
measured data on a different file before restarting the code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Nx 20
#define Ny 20
#define Qmax 20

double erand48(unsigned short xseed[3]);

/*
 
Subroutine update_spins.  This subroutine receives the spins spin of a
q-state Potts model, the inverse temperature beta, the number of states
q, and the seed for the random number generator.  The subroutine returns
in spin the values of the spins after one upgrade of the entire lattice
by the Metropolis Monte Carlo algorithm.  This is obviously the most
important subroutine of the assignment.

In the normal ordering the spins are defined over a rectangular lattice
of Nx by Ny sites with periodic boundary conditions and one can assume
that both Nx and Ny, as passed by the calling program, are powers of 2.
The energy E of the system is defined as the sum of 2*Nx*Ny terms
associated to all the links between nearest neighbors, with the rule
that the links between spins with equal value give no contribution to
the energy, whereas the links between spins with different values give a
contribution to the energy equal to 1.  The probability distribution,
which the algorithm simulates, is then proportional to exp(-beta*E).

The subroutine is self-standing, i.e. it does not use calls to other
subroutines like ``energy'' for calculating the variation of energy
generated by the proposed new value of the spins (in particular, the use
of subroutine ``energy'', which calculates the energy of the whole
lattice rather than just the contributions coming from the four terms
containing a given spin, would be either terribly inefficient or wrong.)

*/

void update_spins(short spin[Nx][Ny], double beta, short q, 
		  unsigned short seed[3])
{
  int x, y;
  short trial_spin;
  double deltaE;

  for (x=0; x<Nx; x++) {
    for (y=0; y<Ny; y++) {

      deltaE = ((spin[x][y]==spin[(x+1)%Nx][y]) + 
		(spin[x][y]==spin[(x-1+Nx)%Nx][y]) +
		(spin[x][y]==spin[x][(y+1)%Ny]) + 
		(spin[x][y]==spin[x][(y-1+Ny)%Ny]));

      trial_spin = (spin[x][y] + 1 + (q-1)*erand48(seed));
      trial_spin = trial_spin%q;      

      deltaE -= ((trial_spin==spin[(x+1)%Nx][y]) + 
		 (trial_spin==spin[(x-1+Nx)%Nx][y]) +
		 (trial_spin==spin[x][(y+1)%Ny]) + 
		 (trial_spin==spin[x][(y-1+Ny)%Ny]));
	
      if (exp(-beta*deltaE)>erand48(seed))
	spin[x][y] = trial_spin;
    }
  }
}

/*
Energy function.  This function receives the array of spins in normal
Cartesian coordinates and the integers Nx and Ny specifing the size of
the rectangular lattice.  The function returns the energy per bond,
where the energy between two spins is defined to be 0 if the two
neighboring spins have the same value, or 1 otherwise.
*/

double energy(short spin[Nx][Ny])
{
  int x, y;
  double E;

  E = 0;

  for (x=0; x<Nx; x++) {
    for (y=0; y<Ny; y++) {
      /* spin to the right */
      E += (1-(spin[x][y]==spin[(x+1)%Nx][y]));
      /* spin above */
      E += (1-(spin[x][y]==spin[x][(y+1)%Ny]));
    }
  }
  return E/(double)(2.*Nx*Ny);
}


/*
Magnetization subroutine.  This subroutine receives the array of spins
in normal Cartesian coordinates the integers Nx and Ny specifing the
size of the rectangular lattice, the number of spin states, and the
array M that will store the magnetization.  The function returns the
magnetization in the array M.  The magnetization is defined as the
fraction of spins in each of the possible spin states.  For example,
if 20% of the system had spin = 0, then M[0] = 0.2.
*/

void magnetization(short spin[Nx][Ny], short q, double M[Qmax])
{
  int s, x, y;

  for (s=0; s<q; s++) {
    M[s] = 0;
  }

  for (x=0; x<Nx; x++) {
    for (y=0; y<Ny; y++) {

      M[spin[x][y]]++;
    }
  }

  for (s=0; s<q; s++) {
    M[s] /= (double)(Nx*Ny);
  }

}

/*
Initialization routine.  This subroutine receives the array of spins in
normal Cartesian coordinates the integers Nx and Ny specifing the size
of the rectangular lattice, the number of spin states, and the seed for
the random number generator.  The function assigns random values in the
range 0->q-1 to each spin
*/
void init_spins(short spin[Nx][Ny], short q, unsigned short seed[3])
{
  int x, y;

  for (x=0; x<Nx; x++) {
    for (y=0; y<Ny; y++) {

      spin[x][y] = q*erand48(seed);

    }
  }
}
