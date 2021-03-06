#ifndef H_NUCLEAR_SURFACE_EN
#define H_NUCLEAR_SURFACE_EN

// LDM
double calc_sly4_ldm_surface_en(double aa_);

// ETF (see: Aymard et al., 2016)
struct f_params
{
    int k;
    double gamma;
}; // integration of Fermi functions
double my_integrand(double x, void *params_ptr);
double eta_function(int a, double b);
double calc_etf_surface_en(struct parameters satdata, double aa_, double ii_, double n0_);

// LS corrected surface energy (see: Lattimer and Swesty, 1991)
double calc_ls_surface_en(struct parameters satdata, double aa_, double ii_);

// LS(ETF) (w/ sigmas and ss fitted out of the ETF numerical calculation)
double calc_bulk_asymmetry(struct parameters satdata, double aa_, double ii_);
double calc_ls_etf_surface_en(struct parameters satdata, double aa_, double ii_, double n0_);

#endif // H_NUCLEAR_SURFACE_EN
