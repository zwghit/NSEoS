#ifndef H_NUCLEAR_EN
#define H_NUCLEAR_EN

#include "nuclear_matter.h"
#include "nuclear_surface_en.h"
#include "coulomb_en.h"

//=============================
//      LDM(SLy4)-ELFc(N)     
//=============================
// bulk: ELFc(N) meta-modeling
// surface: LDM (SLy4 value)
double calc_sly4_ldm_meta_model_nuclear_en(struct parameters satdata, int max_order, 
        double aa_, double ii_, double n0_, double np_);

//=============================
//         ETF-ELFc(N)         
//=============================
// bulk: ELFc(N) meta-modeling
// surface: ETF analytical formula w/ Cfin reference value
double calc_etf_meta_model_nuclear_en(struct parameters satdata, int max_order, 
        double aa_, double ii_, double n0_, double np_);

//=============================
//      LS(SkI')-ELFc(N)     
//=============================
// bulk: ELFc(N) meta-modeling
// surface: LS corrected surface energy w/ SkI' values
double calc_ls_meta_model_nuclear_en(struct parameters satdata, int max_order, 
        double aa_, double ii_, double n0_, double np_);

//=============================
//      LS(ETF)-ELFc(N)     
//=============================
// bulk: ELFc(N) meta-modeling
// surface: LS surface energy w/ parameters 
// ---------fitted out of ETF numerical calculation
double calc_ls_etf_meta_model_nuclear_en(struct parameters satdata, int max_order, 
        double aa_, double ii_, double n0_, double np_);

//=============================
//      DL(ETF)-ELFc(N)     
//=============================
// bulk: ELFc(N) meta-modeling
// surface: DL surface energy w/ parameters 
// ---------fitted out of ETF numerical calculation
double calc_dl_etf_meta_model_nuclear_en(struct parameters satdata, int max_order, 
        double aa_, double ii_, double n0_, double np_);

#endif // H_NUCLEAR_EN
