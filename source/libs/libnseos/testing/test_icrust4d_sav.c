#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>

#include "../nseos/ws_cell.h"

// ==================== FUNCTIONS ====================

struct icrust_fun_4d
{
    double f_stability;
    double f_beta;
    double f_muneq;
    double f_presseq;
};

struct icrust_fun_4d calc_icrust_fun_4d(double aa_, double del_, double rho0_, double rhop_, double rhog_);
struct icrust_fun_4d calc_icrust_fun_4d(double aa_, double del_, double rho0_, double rhop_, double rhog_)
{
    struct icrust_fun_4d result;
    struct parameters satdata;
    double enuc;
    double epsa;
    double epsb;
    double epsr;
    double enuc_ap, enuc_am;
    double enuc_bp, enuc_bm;
    double enuc_rp, enuc_rm;
    double denucdaa;
    double denucddel;
    double denucdrho0;
    struct gas egas;
    struct coulomb_energy_shift coul_shift;
    double dmu;
    struct hnm ngas;

    satdata = assign_param(satdata);

    enuc = calc_enuc(satdata, aa_, del_, rho0_, rhop_);
    epsa = 0.001;
    epsb = 0.0001;
    epsr = 0.0001;
    enuc_ap = calc_enuc(satdata, aa_+epsa, del_, rho0_, rhop_);
    enuc_am = calc_enuc(satdata, aa_-epsa, del_, rho0_, rhop_);
    enuc_bp = calc_enuc(satdata, aa_, del_+epsb, rho0_, rhop_);
    enuc_bm = calc_enuc(satdata, aa_, del_-epsb, rho0_, rhop_);
    enuc_rp = calc_enuc(satdata, aa_, del_, rho0_+epsr, rhop_);
    enuc_rm = calc_enuc(satdata, aa_, del_, rho0_-epsr, rhop_);
    denucdaa = (enuc_ap - enuc_am)/2./epsa; // 2 points derivatives
    denucddel = (enuc_bp - enuc_bm)/2./epsb;
    denucdrho0 = (enuc_rp - enuc_rm)/2./epsr;

    egas = calc_egas(rhop_);
    coul_shift = calc_coulomb_energy_shift(aa_, del_, rho0_, rhop_);
    dmu = coul_shift.derivative;
    ngas = calc_hnm(satdata, rhog_, 1.);

    result.f_stability = denucdaa/aa_ - enuc/aa_/aa_;
    result.f_beta = denucddel*2./aa_ - egas.mutot - dmu - rmp + rmn;
    result.f_muneq = denucdaa - ngas.mug*(1.-rhog_/rho0_) 
        + (1.-del_)/2.*(egas.mutot + dmu + rmp - rmn) - rhog_*ngas.enpernuc/rho0_;
    result.f_presseq = rho0_*rho0_*denucdrho0/aa_ - rhog_*ngas.mug + rhog_*ngas.enpernuc;

    return result;
}

struct rparams
{
    double rhop;
};

int assign_icrust_fun_4d(const gsl_vector * x, void *params, gsl_vector * f);
int assign_icrust_fun_4d(const gsl_vector * x, void *params, gsl_vector * f)
{
    double rhop = ((struct rparams *) params)->rhop;

    const double x0 = gsl_vector_get (x, 0);
    const double x1 = gsl_vector_get (x, 1);
    const double x2 = gsl_vector_get (x, 2);
    const double x3 = gsl_vector_get (x, 3);

    struct parameters satdata;
    satdata = assign_param(satdata);
    rhop = (rhop-x3)*(1.-x1)/2./(1.-x3/x2);

    struct icrust_fun_4d functs;
    functs = calc_icrust_fun_4d(x0, x1, x2, rhop, x3);

    const double y0 = functs.f_stability;
    const double y1 = functs.f_beta;
    const double y2 = functs.f_muneq;
    const double y3 = functs.f_presseq;

    gsl_vector_set(f, 0, y0);
    gsl_vector_set(f, 1, y1);
    gsl_vector_set(f, 2, y2);
    gsl_vector_set(f, 3, y3);

    return GSL_SUCCESS;
}

void print_state_icrust(gsl_multiroot_fsolver * s, double rhob_);
void print_state_icrust(gsl_multiroot_fsolver * s, double rhob_)
{
    double aa_eq, del_eq, rho0_eq, rhog_eq;
    double vws;
    double f0, f1, f2, f3;

    aa_eq = gsl_vector_get(s->x, 0);
    del_eq = gsl_vector_get(s->x, 1);
    rho0_eq = gsl_vector_get(s->x, 2);
    rhog_eq = gsl_vector_get(s->x, 3);
    vws = aa_eq*(1. - rhog_eq/rho0_eq)/(rhob_ - rhog_eq);
    f0 = gsl_vector_get(s->f, 0);
    f1 = gsl_vector_get(s->f, 1);
    f2 = gsl_vector_get(s->f, 2);
    f3 = gsl_vector_get(s->f, 3);

    printf ("%g %g %g %g %g %g %g %g %g %g\n", rhob_, aa_eq, del_eq, rho0_eq, rhog_eq, vws, 
            f0, f1, f2, f3);
}

// ==================== MAIN ====================

int main(void)
{
    /* double irhob; */
    double rhob;
    rhob = 1.8e-4;

    /* for(irhob = 2; irhob <= 25; irhob += 1) */
    /* { */
    const gsl_multiroot_fsolver_type *T;
    gsl_multiroot_fsolver *s;

    int status;
    size_t iter = 0;

    double aa_old, aa_new, astep;
    double basym_old, basym_new, bstep;
    double rho0_old, rho0_new, rstep;
    double rhog_old, rhog_new, gstep;

    /* rhob = irhob/10000.; */
    struct rparams p;
    p.rhop = rhob; // modification in assign_icrust_fun_4d (DIRTY!)

    double x_init[4] = {80., 0.46, 0.09, 0.5*rhob};
    const size_t n = 4;
    gsl_vector *x = gsl_vector_alloc(n);
    gsl_vector_set(x, 0, x_init[0]);
    gsl_vector_set(x, 1, x_init[1]);
    gsl_vector_set(x, 2, x_init[2]);
    gsl_vector_set(x, 3, x_init[3]);

    gsl_multiroot_function f = {&assign_icrust_fun_4d, n, &p};

    T = gsl_multiroot_fsolver_dnewton;
    s = gsl_multiroot_fsolver_alloc(T,4);
    gsl_multiroot_fsolver_set(s, &f, x);

    do
    {
        print_state_icrust(s, rhob);

        aa_old = gsl_vector_get (s->x, 0);
        basym_old = gsl_vector_get (s->x, 1);
        rho0_old = gsl_vector_get (s->x, 2);
        rhog_old = gsl_vector_get (s->x, 3);

        iter++;

        status = gsl_multiroot_fsolver_iterate(s);

        aa_new = gsl_vector_get (s->x, 0);
        basym_new = gsl_vector_get (s->x, 1);
        rho0_new = gsl_vector_get (s->x, 2);
        rhog_new = gsl_vector_get (s->x, 3);
        astep = aa_new - aa_old;
        bstep = basym_new - basym_old;
        rstep = rho0_new - rho0_old;
        gstep = rhog_new - rhog_old;

        if (status)
            break;

        // dirty backstepping
        while (gsl_vector_get (s->x, 0) < 40. || gsl_vector_get (s->x, 0) > 150.) {
            astep = astep/4.;
            aa_new = aa_old + astep;
            gsl_vector_set (x, 0, aa_new);
            gsl_vector_set (x, 1, basym_new);
            gsl_vector_set (x, 2, rho0_new);
            gsl_vector_set (x, 3, rhog_new);
            gsl_multiroot_fsolver_set (s, &f, x);
            aa_new = gsl_vector_get (s->x, 0.);
        }
        while (gsl_vector_get (s->x, 1) < 0.1 || gsl_vector_get (s->x, 1) > 0.7) {
            bstep = bstep/4.;
            basym_new = basym_old + bstep;
            gsl_vector_set (x, 0, aa_new);
            gsl_vector_set (x, 1, basym_new);
            gsl_vector_set (x, 2, rho0_new);
            gsl_vector_set (x, 3, rhog_new);
            gsl_multiroot_fsolver_set (s, &f, x);
            basym_new = gsl_vector_get (s->x, 1);
        }
        while (gsl_vector_get (s->x, 2) < 0.05 || gsl_vector_get (s->x, 2) > 0.1540) {
            rstep = rstep/4.;
            rho0_new = rho0_old + rstep;
            gsl_vector_set (x, 0, aa_new);
            gsl_vector_set (x, 1, basym_new);
            gsl_vector_set (x, 2, rho0_new);
            gsl_vector_set (x, 3, rhog_new);
            gsl_multiroot_fsolver_set (s, &f, x);
            rho0_new = gsl_vector_get (s->x, 2);
        }
        while (gsl_vector_get (s->x, 3) < 0. || gsl_vector_get (s->x, 3) > rhob) {
            gstep = gstep/4.;
            rhog_new = rhog_old + gstep;
            gsl_vector_set (x, 0, aa_new);
            gsl_vector_set (x, 1, basym_new);
            gsl_vector_set (x, 2, rho0_new);
            gsl_vector_set (x, 3, rhog_new);
            gsl_multiroot_fsolver_set (s, &f, x);
            rhog_new = gsl_vector_get (s->x, 3);
        }

        status = gsl_multiroot_test_residual (s->f, 9e-9);
    }

    while (status == GSL_CONTINUE && iter < 5000);

    print_state_icrust(s, rhob);

    gsl_multiroot_fsolver_free(s);
    gsl_vector_free(x);
    /* } */

    return 0;
}
