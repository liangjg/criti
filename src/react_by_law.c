//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"
#include "sample_method.h"


void
react_by_laws(const nuclide_t *nuc,
              RNG_t *RNG,
              int MT,
              int law_type,
              int LDAT,
              double incident_erg,
              double *exit_erg_cm,
              double *exit_mu_cm)
{
    int NR, NE, NET, LN;
    int pos;
    double frac;
    int iter_cnt;

    if(law_type == 1) {
        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        NET = (int) (nuc->XSS[LDAT + LN]);

        double ksi1 = get_rand(RNG);
        int k = 1 + (int) (ksi1 * NET + 1);

        int LC = LDAT + LN + NR + NET * (pos - 1);
        double E_i_1 = nuc->XSS[LC + 1];
        double E_i_K = nuc->XSS[LC + NET];
        LC = LC + NET;
        double E_i1_1 = nuc->XSS[LC + 1];
        double E_i1_K = nuc->XSS[LC + NET];
        double E_1 = E_i_1 + frac * (E_i1_1 - E_i_1);
        double E_K = E_i_K + frac * (E_i1_K - E_i_K);

        double ksi3 = get_rand(RNG);
        int pos_smpl = ksi3 < frac ? pos + 1 : pos;

        LC = LDAT + LN + (pos_smpl - 1) * NET;
        double E_l_k = nuc->XSS[LC + k];
        double E_l_k1 = nuc->XSS[LC + k + 1];
        double ksi2 = get_rand(RNG);
        double E_pie = E_l_k + ksi2 * (E_l_k1 - E_l_k);

        if(pos_smpl == pos)
            *exit_erg_cm = E_1 + (E_pie - E_i_1) * (E_K - E_1) / (E_i_K - E_i_1);
        else
            *exit_erg_cm = E_1 + (E_pie - E_i1_1) * (E_K - E_1) / (E_i1_K - E_i1_1);
    } else if(law_type == 3) {
        *exit_erg_cm = nuc->XSS[LDAT + 1] * (incident_erg - nuc->XSS[LDAT]);
    } else if(law_type == 5) {
        double TI;
        int index;
        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        LN = 2 * (NR + NE + 1);
        TI = get_erg_func_value(nuc, LDAT, incident_erg);
        NET = (int) (nuc->XSS[LDAT + LN]);
        index = LDAT + LN + 1 + (int) (get_rand(RNG) * (NET - 1));
        *exit_erg_cm = TI * (nuc->XSS[index] + get_rand(RNG) * (nuc->XSS[index + 1] - nuc->XSS[index]));
    } else if(law_type == 7) {
        double T, U, CalTemp;

        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        T = get_erg_func_value(nuc, LDAT, incident_erg);
        U = nuc->XSS[LDAT + 2 * (NR + NE + 1)];
        CalTemp = incident_erg - U;
        if(CalTemp > 0) {
            iter_cnt = 0;
            for(;;) {
                *exit_erg_cm = sample_maxwell(RNG, T);
                if(*exit_erg_cm <= CalTemp)
                    break;
                if((iter_cnt++) >= MAX_ITER) {
                    printf("Waring: too many samples. Nuc=%s, LawType=%d, Ein=%f, U=%f, T=%f\n", nuc->id, law_type,
                           incident_erg, U, T);
                    base_warnings++;
                }
            }
        } else {
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 9) {
        double T, U, CalTemp;
        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        LN = 2 * (NR + NE + 1);
        T = get_erg_func_value(nuc, LDAT, incident_erg);
        U = nuc->XSS[LDAT + LN];
        CalTemp = incident_erg - U;
        if(CalTemp > 0) {
            for(;;) {
                double ksi1 = get_rand(RNG);
                double ksi2 = get_rand(RNG);
                *exit_erg_cm = -T * log(ksi1 * ksi2);
                if(*exit_erg_cm <= CalTemp)
                    break;
            }
        } else {
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 11) {
        int NR1 = (int) (nuc->XSS[LDAT]);
        int NE1 = (int) (nuc->XSS[LDAT + 2 * NR1 + 1]);
        int LN1 = 2 * (NR1 + NE1 + 1);
        double a = get_erg_func_value(nuc, LDAT, incident_erg);
        double b = get_erg_func_value(nuc, LDAT + LN1, incident_erg);

        int NR2 = (int) (nuc->XSS[LDAT + LN1]);
        int NE2 = (int) (nuc->XSS[LDAT + LN1 + 2 * NR2 + 1]);
        int LN2 = 2 + 2 * (NR2 + NE2);
        double U = nuc->XSS[LDAT + LN1 + LN2];
        double CalTemp = incident_erg - U;
        if(CalTemp > 0) {
            iter_cnt = 0;
            for(;;) {
                *exit_erg_cm = sample_watt(RNG, a, b);
                if(*exit_erg_cm <= CalTemp)
                    break;
                if((iter_cnt++) >= MAX_ITER) {
                    printf("Waring: too many samples of LawType %d!\n", law_type);
                    base_warnings++;
                }
            }
        } else {
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 22) {
        int ie, NF;

        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        ie = Get_loc_of_DLW(nuc) - 1 + (int) (nuc->XSS[LDAT + LN + pos - 1]);
        NF = (int) (nuc->XSS[ie]);

        iter_cnt = 0;
        do {
            LDAT = ie;
            double fr = get_rand(RNG);
            do {
                LDAT = LDAT + 1;
                fr = fr - nuc->XSS[LDAT];
            } while(fr > 0.);

            if((iter_cnt++) >= MAX_ITER) {
                printf("Waring: too many samples of LawType %d!\n", law_type);
                base_warnings++;
            }
        } while(LDAT > ie + NF);

        *exit_erg_cm = nuc->XSS[LDAT + 2 * NF] * (incident_erg - nuc->XSS[LDAT + NF]);
    } else if(law_type == 24) {
        int index, k;

        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        NET = (int) (nuc->XSS[LDAT + LN]);
        k = (int) (get_rand(RNG) * (NET - 1));
        index = LDAT + LN + 1 + NET * (pos - 1) + k;
        *exit_erg_cm = incident_erg * (nuc->XSS[index] + get_rand(RNG) * (nuc->XSS[index + 1] - nuc->XSS[index]));
    } else if(law_type == 4 || law_type == 44 || law_type == 61) {
        int pos_smpl;
        int LDIS;
        int LC, LC1, LC2;
        int NP1, NP2;
        double E_i_1, E_i_K, E_i1_1, E_i1_K;
        double E_1, E_K;
        int INTTp, NP, INTT, ND;

        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        pos_smpl = frac > get_rand(RNG) ? pos + 1 : pos;

        LDIS = Get_loc_of_DLW(nuc);
        if(MT == -1)
            LDIS = Get_loc_of_DNED(nuc);

        LC1 = LDIS + (int) (nuc->XSS[LDAT - 1 + LN + pos]);

        NP1 = (int) (nuc->XSS[LC1]);
        E_i_1 = nuc->XSS[LC1 + 1];
        E_i_K = nuc->XSS[LC1 + NP1];

        LC2 = LDIS + (int) (nuc->XSS[LDAT - 1 + LN + pos + 1]);
        NP2 = (int) (nuc->XSS[LC2]);
        E_i1_1 = nuc->XSS[LC2 + 1];
        E_i1_K = nuc->XSS[LC2 + NP2];

        E_1 = E_i_1 + frac * (E_i1_1 - E_i_1);
        E_K = E_i_K + frac * (E_i1_K - E_i_K);

        LC = pos_smpl == pos ? LC1 : LC2;

        INTTp = (int) (nuc->XSS[LC - 1]);
        NP = (int) (nuc->XSS[LC]);

        if(INTTp > 10) {
            INTT = INTTp % 10;
            ND = (INTTp - INTT) / 10;
        } else {
            INTT = INTTp;
            ND = 0;
        }

        if(ND > 0) {
            puts("wrong discrete lines in continuous tabular.");
            base_warnings++;
            goto End;
        }

        double r1;
        int ic, ib, ih;
        int k;
        double c_k, E_l_k, p_l_k;
        double KM_R, KM_A;

        r1 = get_rand(RNG);
        ic = LC + 2 * NP + 1;
        ib = LC + 3 * NP;
        while(ib - ic > 1) {
            ih = (ic + ib) / 2;
            if(r1 >= nuc->XSS[ih])
                ic = ih;
            else
                ib = ih;
        }
        k = ic - (LC + 2 * NP);
        k = MIN(k, NP - 1);
        c_k = nuc->XSS[ic];
        E_l_k = nuc->XSS[LC + k];
        p_l_k = nuc->XSS[LC + NP + k];
        KM_R = ZERO;
        KM_A = ZERO;

        if(INTT == 1) {
            if(p_l_k > 0)
                *exit_erg_cm = E_l_k + (r1 - c_k) / p_l_k;
            else
                *exit_erg_cm = E_l_k;
            if(law_type == 44) {
                KM_R = nuc->XSS[LC + 3 * NP + k];
                KM_A = nuc->XSS[LC + 4 * NP + k];
            }
        } else if(INTT == 2) {
            double E_l_k1 = nuc->XSS[LC + k + 1];
            double p_l_k1 = nuc->XSS[LC + NP + k + 1];

            double fr = (p_l_k1 - p_l_k) / (E_l_k1 - E_l_k);
            if(fr == 0)
                *exit_erg_cm = E_l_k + (r1 - c_k) / p_l_k;
            else
                *exit_erg_cm = E_l_k + (sqrt(MAX(0.0, p_l_k * p_l_k + 2 * fr * (r1 - c_k))) - p_l_k) / fr;

            if(law_type == 44) {
                double R_k = nuc->XSS[LC + 3 * NP + k];
                double R_k1 = nuc->XSS[LC + 3 * NP + k + 1];
                double A_k = nuc->XSS[LC + 4 * NP + k];
                double A_k1 = nuc->XSS[LC + 4 * NP + k + 1];

                KM_R = R_k + (R_k1 - R_k) * (*exit_erg_cm - E_l_k) / (E_l_k1 - E_l_k);
                KM_A = A_k + (A_k1 - A_k) * (*exit_erg_cm - E_l_k) / (E_l_k1 - E_l_k);
            }
        } else {
            printf("unknown interpolation type.MT = %d, LDIS = %d, LDAT = %d ,INTTP = %d, INTT = %d, NP = %d\n",
                   MT, LDIS, LDAT, INTTp, INTT, NP);
            base_warnings++;
            goto End;
        }

        if(pos_smpl == pos)
            *exit_erg_cm = E_1 + (*exit_erg_cm - E_i_1) * (E_K - E_1) / (E_i_K - E_i_1);
        else
            *exit_erg_cm = E_1 + (*exit_erg_cm - E_i1_1) * (E_K - E_1) / (E_i1_K - E_i1_1);

        if(law_type == 44) {
            double ksi3 = get_rand(RNG);
            double ksi4 = get_rand(RNG);
            if(ksi3 > KM_R) {
                double T = (2 * ksi4 - 1) * sinh(KM_A);
                *exit_mu_cm = log(T + sqrt((T * T) + 1)) / KM_A;
            } else
                *exit_mu_cm = log(ksi4 * exp(KM_A) + (1 - ksi4) * exp(-KM_A)) / KM_A;
            return;
        } else if(law_type == 61) {
            int LB, LM, JJ;
            double r3;
            double p_k, mu_k;

            LB = ic + NP;
            if(INTT != 1 && (nuc->XSS[ib] - r1 < r1 - nuc->XSS[ic]))
                LB = LB + 1;
            LM = (int) (nuc->XSS[LB]);

            if(LM == 0) {
                *exit_mu_cm = 2 * get_rand(RNG) - 1;
                return;
            }
            LC = Get_loc_of_DLW(nuc) + abs(LM) - 2;
            JJ = (int) (nuc->XSS[LC + 1]);
            NP = (int) (nuc->XSS[LC + 2]);

            r3 = get_rand(RNG);
            c_k = nuc->XSS[LC + 2 + 2 * NP + 1];
            for(k = 1; k <= NP - 1; ++k) {
                double temp = nuc->XSS[LC + 2 + 2 * NP + k + 1];
                if(r3 < temp)
                    break;
                c_k = temp;
            }
            k = MIN(k, NP - 1);
            p_k = nuc->XSS[LC + 2 + NP + k];
            mu_k = nuc->XSS[LC + 2 + k];
            if(JJ == 1) {
                if(p_k > 0)
                    *exit_mu_cm = mu_k + (r3 - c_k) / p_k;
                else
                    *exit_mu_cm = mu_k;
            } else if(JJ == 2) {
                double p_k1 = nuc->XSS[LC + 2 + NP + k + 1];
                double mu_k1 = nuc->XSS[LC + 2 + k + 1];

                double fr = (p_k1 - p_k) / (mu_k1 - mu_k);
                if(fr == 0)
                    *exit_mu_cm = mu_k + (r3 - c_k) / p_k;
                else
                    *exit_mu_cm = mu_k + (sqrt(p_k * p_k + 2 * fr * (r3 - c_k)) - p_k) / fr;
            } else {
                puts("unknown interpolation type.");
                base_warnings++;
                goto End;
            }
        }
    } else if(law_type == 66) {
        int NPSX;
        double Ap, aw, Q, E_max;
        double lx, ly, V;

        NPSX = (int) (nuc->XSS[LDAT]);
        Ap = nuc->XSS[LDAT + 1];
        aw = nuc->atom_wgt;
        Q = Get_nuc_fis_Q(nuc, MT);
        E_max = ((Ap - 1) / Ap) * (incident_erg * aw / (aw + 1) + Q);

        lx = sample_maxwell(RNG, 1);
        if(NPSX == 3)
            ly = sample_maxwell(RNG, 1);
        else if(NPSX == 4) {
            double temp = get_rand(RNG) * get_rand(RNG) * get_rand(RNG);
            ly = log(temp);
        } else {
            double r1 = get_rand(RNG);
            double r2 = get_rand(RNG);
            double r3 = get_rand(RNG);
            double r4 = get_rand(RNG);
            double r5 = get_rand(RNG);
            double r6 = get_rand(RNG);
            ly = -log(r1 * r2 * r3 * r4) - log(r5) * cos(PI / TWO * r6) * cos(PI / TWO * r6);
        }
        V = lx / (lx + ly);
        *exit_erg_cm = V * E_max;
        *exit_mu_cm = TWO * get_rand(RNG) - ONE;
    } else if(law_type == 67) {
        int LC[3];
        int ix, ir;
        int i, j;
        double E_i_1[3], E_i_K[3];
        double E_1, E_K;

        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = LDAT + 2 * NR + NE + 2;

        LC[1] = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[LN + pos - 1]) - 1;
        LC[2] = 0;

        ix = 1;
        ir = 0;
        if(frac != 0) {
            LC[2] = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[LN + pos]) - 1;
            ir = 1;
            if(get_rand(RNG) <= frac)
                ix = 2;
        }

        for(i = 1; i <= 2; i++) {
            int le, INTMU, NMU, LMU;

            if(LC[i] == 0)
                continue;
            le = LC[i];
            INTMU = (int) (nuc->XSS[le]);
            NMU = (int) (nuc->XSS[le + 1]);
            LMU = 0;
            for(j = 1; j <= NMU - 1; j++) {
                if(*exit_mu_cm <= nuc->XSS[le + j + 2]) {
                    LMU = j;
                    break;
                }
            }
            if(INTMU == 2) {
                ir = ir + 1;
                if(get_rand(RNG) <
                   (*exit_mu_cm - nuc->XSS[le + LMU + 1]) / (nuc->XSS[le + LMU + 2] - nuc->XSS[le + LMU + 1]))
                    LMU = LMU + 1;
            } else if(INTMU != 1) {
                printf("unknown INTMU interpolation type %d.\n", INTMU);
                base_warnings++;
                goto End;
            }

            int lb, INTEP, NPEP, ib, ic;
            double ksi4;
            double E_l_m_k, E_l_m_kk, p_l_m_k, p_l_m_kk, c_l_m_k;

            lb = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[le + LMU + NMU + 1]);
            INTEP = (int) (nuc->XSS[lb - 1]);
            NPEP = (int) (nuc->XSS[lb]);
            E_i_1[i] = nuc->XSS[lb + 1];
            E_i_K[i] = nuc->XSS[lb + NPEP];
            if(i != ix)
                continue;

            ir = ir + 1;
            ksi4 = get_rand(RNG);
            ic = lb + 2 * NPEP + 1;
            ib = lb + 3 * NPEP;

            while(ib - ic > 1) {
                int ih = (ic + ib) / 2;
                if(ksi4 >= nuc->XSS[ih])
                    ic = ih;
                else
                    ib = ih;
            }

            E_l_m_k = nuc->XSS[ic - 2 * NPEP];
            E_l_m_kk = nuc->XSS[ic - 2 * NPEP + 1];
            p_l_m_k = nuc->XSS[ic - NPEP];
            p_l_m_kk = nuc->XSS[ic - NPEP + 1];
            c_l_m_k = nuc->XSS[ic];

            if(INTEP == 1)
                *exit_erg_cm = E_l_m_k + (ksi4 - c_l_m_k) / p_l_m_k;
            else if(INTEP == 2) {
                double cal_temp1 = (p_l_m_kk - p_l_m_k) / (E_l_m_kk - E_l_m_k);
                if(cal_temp1 == 0)
                    *exit_erg_cm = E_l_m_k + (ksi4 - c_l_m_k) / p_l_m_k;
                else {
                    double cal_temp2 = sqrt(MAX(0.0, (p_l_m_k * p_l_m_k) + 2 * cal_temp1 * (ksi4 - c_l_m_k)));
                    *exit_erg_cm = E_l_m_k + (cal_temp2 - p_l_m_k) / cal_temp1;
                }
            } else {
                puts("unknown interpolation type.");
                base_warnings++;
                goto End;
            }
        }

        if(frac == 0)
            return;

        E_1 = E_i_1[1] + frac * (E_i_1[2] - E_i_1[1]);
        E_K = E_i_K[1] + frac * (E_i_K[2] - E_i_K[1]);
        *exit_erg_cm = E_1 + (*exit_erg_cm - E_i_1[ix]) * (E_K - E_1) / (E_i_K[ix] - E_i_1[ix]);
    } else {
        printf("unknown Law type.Law type = %d.\n", law_type);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }

End:
    if(MT == -1 && law_type != 44 && law_type != 61)
        return;

    if(nuc->LAND[MT] < 0 && law_type != 44 && law_type != 61 && law_type != 66) {
        printf("angular distribution is not processed for LAND[MT] = -1. LAW = %d\n", law_type);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }

    if(*exit_erg_cm == 0) {
        *exit_erg_cm = ZERO_ERG;//
    } else if(!(*exit_erg_cm > 0 && *exit_erg_cm < 100)) {
        printf("exit energy_cm out of range.\n        nuc=%s, MT=%d, Law=%d, Erg=%9.6E \n",
               nuc->id, MT, law_type, *exit_erg_cm);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }

    if(!(*exit_mu_cm >= -1.000001 && *exit_mu_cm <= 1.000001)) {
        printf("exit mu_cm out of range. nuc=%d, MT=%d, Law=%d, Mu=%f \n", nuc->zaid, MT, law_type, *exit_mu_cm);
        base_warnings++;
        *exit_mu_cm = 2 * get_rand(RNG) - 1.;
    }
}