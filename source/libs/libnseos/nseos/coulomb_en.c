#include <math.h>
#include <complex.h>

#include "mathconst.h"
#include "phyconst.h"
#include "coulomb_en.h"

double calc_coulomb_en(struct parameters satdata, double aa_, double ii_, double n0_, double np_)
{
    double rsat;
    double ac;
    double rpt;
    double fws; // screening function
    double zz;

    rsat = pow(3./4./PI/satdata.rhosat0,1./3.);
    ac = 3./5.*ALPHAFS*HBARC/rsat;
    rpt = 2.*np_/(1.-ii_)/n0_;
    fws = -1.5*pow(rpt,1./3.) + 0.5*rpt;
    zz = aa_*(1.-ii_)/2.;

    return ac*zz*zz*pow(aa_,-1./3.)*(1.+fws);
}

double calc_screening_derivative(struct parameters satdata, double aa_, double ii_, double n0_, double np_)
{
    double rsat;
    double ac;
    double rpt;

    rsat = pow(3./4./PI/satdata.rhosat0,1./3.);
    ac = 3./5.*ALPHAFS*HBARC/rsat;
    rpt = 2.*np_/(1.-ii_)/n0_;

    return 0.5*ac*pow(aa_,2./3.)*np_/n0_*(pow(rpt,-2./3.) - 1.);
}

double calc_egas_energy_density(double np_)
{
    double t;
    double egas_energy_density;

    t = HBARC*cpow(3.0*PI2*np_,1./3.)/MEL;
    egas_energy_density = cpow(MEL,4.)/8./PI2/cpow(HBARC,3.)*((2.*t*t+1.)*t*cpow(t*t+1.,1./2.) - log(t + cpow(t*t+1.,1./2.)));

    return egas_energy_density;
}

double calc_egas_chemical_potential(double np_)
{
    double t;
    double egas_chemical_potential;

    t = HBARC*cpow(3.0*PI2*np_,1./3.)/MEL;
    egas_chemical_potential = cpow(MEL,3.)/8./cpow(3.*np_*PI2,2./3.)/cpow(HBARC,2.)
        *(cpow(t*t+1.,1./2.)*(1.+6.*t*t) + t*t*(2.*t*t+1.)/cpow(t*t+1.,1./2.)
                - 1./cpow(t*t+1.,1./2.));

    return egas_chemical_potential;
}
