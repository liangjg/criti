//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "RNG.h"

void treat_sab_colli_type(acedata_t *obj, int nSabColliNuc, double dSIG_sab_el, double dSIG_sab_inel, double dEin,
                          double *dDirectin, double &dExitErgInLab, double *dDirectout)
{
    int sab_n_el, sab_n_inel;
    double sab_k_el, sab_k_inel;
    double dMuInLab;

    ////////////////// elastic scattering case.  JXS(4) = 0 : without elastic scattering ////////////////
    if(GetLocOfSabElErg(obj, nSabColliNuc) != 0 && get_rand() * (dSIG_sab_el + dSIG_sab_inel) > dSIG_sab_inel) {
        dExitErgInLab = dEin;

        int NE_el = int(obj->nucs[nSabColliNuc]->XSS[GetLocOfSabElErg(obj, nSabColliNuc)]);
        int min = GetLocOfSabElErg(obj, nSabColliNuc) + 1;
        int max = GetLocOfSabElErg(obj, nSabColliNuc) + NE_el;
        double f_min = obj->nucs[nSabColliNuc]->XSS[min];
        double f_max = obj->nucs[nSabColliNuc]->XSS[max];

        get_intplt_pos_fr_double(obj->nucs[nSabColliNuc]->XSS, dEin, min, max, &sab_n_el, &sab_k_el); //// sab_n_el never reach max
        sab_n_el = sab_n_el - GetLocOfSabElErg(obj, nSabColliNuc);
        int LN = GetLocOfSabElMu(obj, nSabColliNuc) + (sab_n_el - 1) * (abs(GetSabElDimPara(obj, nSabColliNuc)) + 1);
        int N_el_mu = GetSabElDimPara(obj, nSabColliNuc) + 1;
        int Loc;
        int nMode_el = GetSabElMode(obj, nSabColliNuc);
        if(nMode_el == 2) {     ////// equally-probable angle bins.
            if(get_rand() <= sab_k_el)
                LN = LN + N_el_mu;
            double ksi = (N_el_mu - 1) * get_rand() + 1;
            int Loc = int(LN + ksi);
            dMuInLab = obj->nucs[nSabColliNuc]->XSS[Loc] + (obj->nucs[nSabColliNuc]->XSS[Loc - 1] - obj->nucs[nSabColliNuc]->XSS[Loc]) * (ksi - int(ksi));
        } else if(nMode_el == 3) { /////// qually-probable discrete angles.
            Loc = LN + int(N_el_mu * get_rand());
            dMuInLab = obj->nucs[nSabColliNuc]->XSS[Loc] + sab_k_el * (obj->nucs[nSabColliNuc]->XSS[Loc + N_el_mu] - obj->nucs[nSabColliNuc]->XSS[Loc]);
        } else if(nMode_el == 4) {
            min = GetLocOfSabElXs(obj, nSabColliNuc);
            max = GetLocOfSabElXs(obj, nSabColliNuc) + sab_n_el;
            double pi = obj->nucs[nSabColliNuc]->XSS[max] * get_rand();
            get_intplt_pos_double(obj->nucs[nSabColliNuc]->XSS, pi, min, max, &Loc);
            Loc = Loc + 1 - int(obj->nucs[nSabColliNuc]->XSS[GetLocOfSabElErg(obj, nSabColliNuc)]);
            dMuInLab = 1 - 2.0 * obj->nucs[nSabColliNuc]->XSS[Loc] / dEin;
        } else {
            printf("incorrect elastic scattering mode(%d) in sab collision.\n", nMode_el);
            base_warnings++;
        }
    }

        ////////////////// inelastic scattering case.  JXS(4) = 0 : without elastic scattering ////////////////
    else {
        int NE_inel = int(obj->nucs[nSabColliNuc]->XSS[GetLocOfSabInelErg(obj, nSabColliNuc)] + 0.5);
        int min = GetLocOfSabInelErg(obj, nSabColliNuc) + 1;
        int max = GetLocOfSabInelErg(obj, nSabColliNuc) + NE_inel;
        get_intplt_pos_fr_double(obj->nucs[nSabColliNuc]->XSS, dEin, min, max, &sab_n_inel, &sab_k_inel);
        sab_n_inel = sab_n_inel - GetLocOfSabInelErg(obj, nSabColliNuc);

        int LE;
        if(GetSabSecErgMode(obj, nSabColliNuc) == 0)
            LE = int(get_rand() * GetSabInelEoutNum(obj, nSabColliNuc));
        else {
            double rr = get_rand() * (GetSabInelEoutNum(obj, nSabColliNuc) - 3);
            if(rr >= 1.0)
                LE = int(rr + 1);
            else {
                if(rr >= 0.5) {
                    LE = GetSabInelEoutNum(obj, nSabColliNuc) - 2;
                    if(rr < 0.6)
                        LE = LE + 1;
                } else {
                    LE = 1;
                    if(rr < 0.1)
                        LE = 0;
                }
            }
        }
        int N_inel_mu = GetSabInelDimPara(obj, nSabColliNuc) + 1;
        int LN_offset = GetSabInelEoutNum(obj, nSabColliNuc) * (N_inel_mu + 1); // ��ֵ��λ��ƫ��
        int LN = (sab_n_inel - 1) * LN_offset + GetLocOfSabInelErgMu(obj, nSabColliNuc) + LE * (N_inel_mu + 1);
        dExitErgInLab = obj->nucs[nSabColliNuc]->XSS[LN] + sab_k_inel * (obj->nucs[nSabColliNuc]->XSS[LN + LN_offset] - obj->nucs[nSabColliNuc]->XSS[LN]);
        if(dExitErgInLab <= 0) {
            puts("exit energy in sab collision is out of range.");
            base_warnings++;
            dExitErgInLab = EG0_CUTOFF;
        } else if(dExitErgInLab <= 1.0E-11) {
            if(dExitErgInLab > EG0_CUTOFF)
                dExitErgInLab = 1.0E-11;
        }



        LN = LN + 1;
        int Mode_inel = GetSabInelMode(obj, nSabColliNuc);
        if(Mode_inel == 2) {  // equally-probable angle bins.
            if(get_rand() <= sab_k_inel)
                LN = LN + LN_offset;
            double ksi = (N_inel_mu - 1) * get_rand() + 1;
            int Loc = int(LN + ksi);
            dMuInLab = obj->nucs[nSabColliNuc]->XSS[Loc] + (obj->nucs[nSabColliNuc]->XSS[Loc - 1] - obj->nucs[nSabColliNuc]->XSS[Loc]) * (ksi - int(ksi));
        } else if(Mode_inel == 3) { // qually-probable discrete angles.
            int Loc = LN + int(N_inel_mu * get_rand());
            dMuInLab = obj->nucs[nSabColliNuc]->XSS[Loc] + sab_k_inel * (obj->nucs[nSabColliNuc]->XSS[Loc + LN_offset] - obj->nucs[nSabColliNuc]->XSS[Loc]);
        } else {
            printf("incorrect inelastic scattering mode(%d) in sab collision.\n", Mode_inel);
            base_warnings++;
        }
    }


    //////////// check outgoing angle and energy //////////////////////
    dMuInLab = MIN(ONE, dMuInLab);
    dMuInLab = MAX(-ONE, dMuInLab);
//    cNeutronTransport.RotateDirection(dMuInLab, dDirectin, dDirectout);
}