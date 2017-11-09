//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void interpolate_sab(acedata_t *obj, int nNuclide_n, int nNuclide_sab, double dEg0)
{
    int min, max;
    int nNE_el, nNE_inel;
    int nsab_n_el, nsab_n_inel;
    double dSIG_sab_el, dSIG_sab_inel, dsab_k_el, dsab_k_inel;

    /////////////// calculate the s(a,b) inelastic scattering cross section ///////////////
    nNE_inel = int(obj->nucs[nNuclide_sab]->XSS[GetLocOfSabInelErg(obj, nNuclide_sab)]);
    min = GetLocOfSabInelErg(obj, nNuclide_sab) + 1;
    max = GetLocOfSabInelErg(obj, nNuclide_sab) + nNE_inel;
    get_intplt_pos_fr_double(obj->nucs[nNuclide_sab]->XSS, dEg0, min, max, &nsab_n_inel, &dsab_k_inel);

    dSIG_sab_inel = obj->nucs[nNuclide_sab]->XSS[nNE_inel + nsab_n_inel] +
                    dsab_k_inel * (obj->nucs[nNuclide_sab]->XSS[nNE_inel + nsab_n_inel + 1] - obj->nucs[nNuclide_sab]->XSS[nNE_inel + nsab_n_inel]);


    /////////////// calculate the s(a,b) elastic scattering cross section ///////////////
    if(GetLocOfSabElErg(obj, nNuclide_sab) == 0)
        dSIG_sab_el = 0;
    else {
        nNE_el = int(obj->nucs[nNuclide_sab]->XSS[GetLocOfSabElErg(obj, nNuclide_sab)]);
        min = GetLocOfSabElErg(obj, nNuclide_sab) + 1;

        if(GetSabElMode(obj, nNuclide_sab) == 4 && dEg0 <= obj->nucs[nNuclide_sab]->XSS[min])
            dSIG_sab_el = 0;
        else {
            max = GetLocOfSabElErg(obj, nNuclide_sab) + nNE_el;
            get_intplt_pos_fr_double(obj->nucs[nNuclide_sab]->XSS, dEg0, min, max, &nsab_n_el, &dsab_k_el);
            if(GetSabElMode(obj, nNuclide_sab) == 4)
                dSIG_sab_el = obj->nucs[nNuclide_sab]->XSS[nNE_el + nsab_n_el] / dEg0;
            else {
                dSIG_sab_el = obj->nucs[nNuclide_sab]->XSS[nNE_el + nsab_n_el] +
                              dsab_k_el * (obj->nucs[nNuclide_sab]->XSS[nNE_el + nsab_n_el + 1] - obj->nucs[nNuclide_sab]->XSS[nNE_el + nsab_n_el]);
            }
        }
    }


    //if(Nuclides[nuclide_sab]->ProbTableFlag==1){printf("TreatURR conflict with sab \n");}
    int nn = obj->nucs[nNuclide_n]->inter_pos;
    double kk = obj->nucs[nNuclide_n]->inter_frac;
    int NE = GetErgGridNum(obj, nNuclide_n);
    obj->nucs[nNuclide_n]->abs = obj->nucs[nNuclide_n]->XSS[nn + 2 * NE] + kk * (obj->nucs[nNuclide_n]->XSS[nn + 2 * NE + 1] - obj->nucs[nNuclide_n]->XSS[nn + 2 * NE]);
    obj->nucs[nNuclide_n]->tot = obj->nucs[nNuclide_n]->abs + dSIG_sab_el + dSIG_sab_inel;
    obj->nucs[nNuclide_n]->el = dSIG_sab_el;
    obj->nucs[nNuclide_n]->inel = dSIG_sab_inel;
    obj->nucs[nNuclide_n]->fis = 0;
}