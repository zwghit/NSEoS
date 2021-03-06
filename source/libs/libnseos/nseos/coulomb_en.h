#ifndef H_COULOMB_EN
#define H_COULOMB_EN

#include "empirical.h"

double calc_coulomb_en(struct parameters satdata, double aa_, double ii_, double n0_, double np_);
double calc_screening_derivative(struct parameters satdata, double aa_, double ii_, double n0_, double np_);
double calc_egas_energy_density(double np_);
double calc_egas_chemical_potential(double np_);

#endif // H_COULOMB_EN
