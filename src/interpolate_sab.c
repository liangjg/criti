//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void interpolate_sab(nuclide_t *nuc, nuclide_t *sab_nuc, double erg){
    int min, max;
    int nNE_el, nNE_inel;
    int nsab_n_el, nsab_n_inel;
    double dSIG_sab_el, dSIG_sab_inel, dsab_k_el, dsab_k_inel;

    /////////////// calculate the s(a,b) inelastic scattering cross section ///////////////
    nNE_inel = (int)(sab_nuc->XSS[GetLocOfSabInelErg(sab_nuc)]);
    min = GetLocOfSabInelErg(sab_nuc) + 1;
    max = GetLocOfSabInelErg(sab_nuc) + nNE_inel;
    get_intplt_pos_fr_double(sab_nuc->XSS, erg, min, max, &nsab_n_inel, &dsab_k_inel);

    dSIG_sab_inel = sab_nuc->XSS[nNE_inel + nsab_n_inel] +
                    dsab_k_inel * (sab_nuc->XSS[nNE_inel + nsab_n_inel + 1] - sab_nuc->XSS[nNE_inel + nsab_n_inel]);


    /////////////// calculate the s(a,b) elastic scattering cross section ///////////////
    if(GetLocOfSabElErg(sab_nuc) == 0)
        dSIG_sab_el = 0;
    else{
        nNE_el = (int)(sab_nuc->XSS[GetLocOfSabElErg(sab_nuc)]);
        min = GetLocOfSabElErg(sab_nuc) + 1;

        if(GetSabElMode(sab_nuc) == 4 && erg <= sab_nuc->XSS[min])
            dSIG_sab_el = 0;
        else{
            max = GetLocOfSabElErg(sab_nuc) + nNE_el;
            get_intplt_pos_fr_double(sab_nuc->XSS, erg, min, max, &nsab_n_el, &dsab_k_el);
            if(GetSabElMode(sab_nuc) == 4)
                dSIG_sab_el = sab_nuc->XSS[nNE_el + nsab_n_el] / erg;
            else{
                dSIG_sab_el = sab_nuc->XSS[nNE_el + nsab_n_el] +
                              dsab_k_el * (sab_nuc->XSS[nNE_el + nsab_n_el + 1] - sab_nuc->XSS[nNE_el + nsab_n_el]);
            }
        }
    }

    //if(Nuclides[nuclide_sab]->ProbTableFlag==1){printf("TreatURR conflict with sab \n");}
    int nn = nuc->inter_pos;
    double kk = nuc->inter_frac;
    int NE = GetErgGridNum(nuc);
    nuc->abs = nuc->XSS[nn + 2 * NE] + kk * (nuc->XSS[nn + 2 * NE + 1] - nuc->XSS[nn + 2 * NE]);
    nuc->tot = nuc->abs + dSIG_sab_el + dSIG_sab_inel;
    nuc->el = dSIG_sab_el;
    nuc->inel = dSIG_sab_inel;
    nuc->fis = 0;
}