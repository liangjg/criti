//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"
#include "sample_method.h"

/// 根据能角不耦合的Law确定质心系下出射能量和出射角余弦
void react_by_laws(const nuclide_t *nuc, int MT, int law_type, int LDAT, double incident_erg,
                   double *exit_erg_cm, double *exit_mu_cm){
    int NR, NE, NET, LN;
    int pos;
    double frac;

    int iter_cnt = 0;

    if(law_type == 1){ // TABULAR EQUIPROBABLE ENERGY BINS
        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        NET = (int) (nuc->XSS[LDAT + LN]);

        //// Sample outgoing energy bin
        double ksi1 = get_rand();
        int k = 1 + (int) (ksi1 * NET + 1);

        //// calculate E_1 and E_K
        int LC = LDAT + LN + NR + NET * (pos - 1);
        double E_i_1 = nuc->XSS[LC + 1];
        double E_i_K = nuc->XSS[LC + NET];
        LC = LC + NET;
        double E_i1_1 = nuc->XSS[LC + 1];
        double E_i1_K = nuc->XSS[LC + NET];
        double E_1 = E_i_1 + frac * (E_i1_1 - E_i_1);
        double E_K = E_i_K + frac * (E_i1_K - E_i_K);

        //// Sample outgoing table for incoming energy E_i and E_(i+1)
        double ksi3 = get_rand();
        int pos_smpl = ksi3 < frac ? pos + 1 : pos;

        //// Calculate E_l_k, E_l_k+1 and E'
        LC = LDAT + LN + (pos_smpl - 1) * NET;
        double E_l_k = nuc->XSS[LC + k];
        double E_l_k1 = nuc->XSS[LC + k + 1];
        double ksi2 = get_rand();
        double E_pie = E_l_k + ksi2 * (E_l_k1 - E_l_k);

        //// scaled interpolation between tables
        if(pos_smpl == pos)
            *exit_erg_cm = E_1 + (E_pie - E_i_1) * (E_K - E_1) / (E_i_K - E_i_1);
        else
            *exit_erg_cm = E_1 + (E_pie - E_i1_1) * (E_K - E_1) / (E_i1_K - E_i1_1);
    } else if(law_type == 3){ // INELASTIC LEVEL SCATTERING
        *exit_erg_cm = nuc->XSS[LDAT + 1] * (incident_erg - nuc->XSS[LDAT]);
    } else if(law_type == 5){ // GENERAL EVAPORATION SPECTRUM
        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        LN = 2 * (NR + NE + 1);
        double TI = get_erg_func_value(nuc, LDAT, incident_erg);
        int NET = (int) (nuc->XSS[LDAT + LN]);
        int i = LDAT + LN + 1 + (int) (get_rand() * (NET - 1));
        *exit_erg_cm = TI * (nuc->XSS[i] + get_rand() * (nuc->XSS[i + 1] - nuc->XSS[i]));
    } else if(law_type == 7){ // MAXWELL FISSION SPECTRUM
        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        double T = get_erg_func_value(nuc, LDAT, incident_erg);
        double U = nuc->XSS[LDAT + 2 * (NR + NE + 1)];
        double CalTemp = incident_erg - U;
        if(CalTemp > 0){
            iter_cnt = 0;
            for(;;){
                *exit_erg_cm = sample_maxwell(T);
                if(*exit_erg_cm <= CalTemp)
                    break;
                if((iter_cnt++) >= MAX_ITER){
                    printf("Waring: too many samples. Nuc=%s, LawType=%d, Ein=%f, U=%f, T=%f\n", nuc->id, law_type,
                           incident_erg, U, T);
                    base_warnings++;
                }
            }
        } else{
            //Output.OutputMessage([&](){sprintf(Output.PrintStr,"Waring: Ein %f < U %f.Nuc=%s, LawType=%d!\n",Ein,U, Nuclides[nuc]->NucID,LawType);},CD_Output::_WARNING);
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 9){ // EVAPORATION SPECTRUM
        NR = (int) (nuc->XSS[LDAT]);
        NE = (int) (nuc->XSS[LDAT + 2 * NR + 1]);
        LN = 2 * (NR + NE + 1);
        double T = get_erg_func_value(nuc, LDAT, incident_erg);
        double U = nuc->XSS[LDAT + LN];
        double dCalcTemp = incident_erg - U;
        if(dCalcTemp > 0){
            iter_cnt = 0;
            for(;;){
                double ksi1 = get_rand();
                double ksi2 = get_rand();
                *exit_erg_cm = -T * log(ksi1 * ksi2);
                if(*exit_erg_cm <= dCalcTemp)
                    break;
            }
        } else{
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 11){ // ENERGY-DEPENDENT WATT SPECTRUM
        int NR1 = (int) (nuc->XSS[LDAT]);
        int NE1 = (int) (nuc->XSS[LDAT + 2 * NR1 + 1]);
        int LN1 = 2 * (NR1 + NE1 + 1);
        double a = get_erg_func_value(nuc, LDAT, incident_erg); // watt parameter a
        double b = get_erg_func_value(nuc, LDAT + LN1, incident_erg); // watt parameter b

        int NR2 = (int) (nuc->XSS[LDAT + LN1]);
        int NE2 = (int) (nuc->XSS[LDAT + LN1 + 2 * NR2 + 1]);
        int LN2 = 2 + 2 * (NR2 + NE2);
        double U = nuc->XSS[LDAT + LN1 + LN2]; // restriction energy
        double dCalTemp = incident_erg - U;
        if(dCalTemp > 0){
            iter_cnt = 0;
            for(;;){
                *exit_erg_cm = sample_watt(a, b);
                if(*exit_erg_cm <= dCalTemp)
                    break;
                if((iter_cnt++) >= MAX_ITER){
                    printf("Waring: too many samples of LawType %d!\n", law_type);
                    base_warnings++;
                }
            }
        } else{
            *exit_erg_cm = ZERO_ERG;
        }
    } else if(law_type == 22){
        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        int ie = Get_loc_of_DLW(nuc) - 1 + (int) (nuc->XSS[LDAT + LN + pos - 1]);
        int NF = (int) (nuc->XSS[ie]);

        iter_cnt = 0;
        do{
            LDAT = ie;
            double fr = get_rand();
            do{
                LDAT = LDAT + 1;
                fr = fr - nuc->XSS[LDAT];
            } while(fr > 0.);

            if((iter_cnt++) >= MAX_ITER){
                printf("Waring: too many samples of LawType %d!\n", law_type);
                base_warnings++;
            }
        } while(LDAT > ie + NF);

        *exit_erg_cm = nuc->XSS[LDAT + 2 * NF] * (incident_erg - nuc->XSS[LDAT + NF]);
    } else if(law_type == 24){
        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = 2 * NR + NE + 2;
        NET = (int) (nuc->XSS[LDAT + LN]);
        int k = (int) (get_rand() * (NET - 1));
        int i = LDAT + LN + 1 + NET * (pos - 1) + k;
        *exit_erg_cm = incident_erg * (nuc->XSS[i] + get_rand() * (nuc->XSS[i + 1] - nuc->XSS[i]));
    } else if(law_type == 4 || law_type == 44 || law_type == 61){

        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);


        LN = 2 * NR + NE + 2;

        //// Sample between the ith and (i+1)th bin
        int nPos_smpl = frac > get_rand() ? pos + 1 : pos;

        //// obtain endpoints on grid i
        int LDIS = Get_loc_of_DLW(nuc);
        if(MT == -1)
            LDIS = Get_loc_of_DNED(nuc);

        int LC1 = LDIS + (int) (nuc->XSS[LDAT - 1 + LN + pos]);

        int NP1 = (int) (nuc->XSS[LC1]);
        double E_i_1 = nuc->XSS[LC1 + 1];
        double E_i_K = nuc->XSS[LC1 + NP1];

        //// obtain endpoints on grid i+1
        int LC2 = LDIS + (int) (nuc->XSS[LDAT - 1 + LN + pos + 1]);
        int NP2 = (int) (nuc->XSS[LC2]);
        double E_i1_1 = nuc->XSS[LC2 + 1];
        double E_i1_K = nuc->XSS[LC2 + NP2];

        double E_1 = E_i_1 + frac * (E_i1_1 - E_i_1);
        double E_K = E_i_K + frac * (E_i1_K - E_i_K);

        //// obtain location of outgoing energies, pdf, cdf for E(l)
        int LC = nPos_smpl == pos ? LC1 : LC2;

        //// obtain type of interpolation and number of discrete lines
        int INTTp = (int) (nuc->XSS[LC - 1]);
        int NP = (int) (nuc->XSS[LC]);

        int INTT = 0;
        int ND = 0;
        if(INTTp > 10){
            INTT = INTTp % 10;
            ND = (INTTp - INTT) / 10;
        } else{
            INTT = INTTp;
            ND = 0;
        }

        if(ND > 0){
            puts("wrong discrete lines in continuous tabular.");
            base_warnings++;
            goto End;
        }

        //printf("INTTP = %d, INTT = %d, NP = %d\n",INTTp,INTT,NP);
        //// sample outgoing energy bin
        double r1 = get_rand();
        int ic = LC + 2 * NP + 1;
        int ib = LC + 3 * NP;
        int ih;
        while(ib - ic > 1){
            ih = (ic + ib) / 2;
            if(r1 >= nuc->XSS[ih])
                ic = ih;
            else
                ib = ih;
        }
        int k = ic - (LC + 2 * NP);
        k = MIN(k, NP - 1);

        double c_k = nuc->XSS[ic];
        double E_l_k = nuc->XSS[LC + k];
        double p_l_k = nuc->XSS[LC + NP + k];
        double KM_R, KM_A;
        if(INTT == 1){ // Histogram interpolation
            if(p_l_k > 0)
                *exit_erg_cm = E_l_k + (r1 - c_k) / p_l_k;
            else
                *exit_erg_cm = E_l_k;
            if(law_type == 44){
                KM_R = nuc->XSS[LC + 3 * NP + k];
                KM_A = nuc->XSS[LC + 4 * NP + k];
            }
        } else if(INTT == 2){ //Linear-linear interpolation
            double E_l_k1 = nuc->XSS[LC + k + 1];
            double p_l_k1 = nuc->XSS[LC + NP + k + 1];

            double fr = (p_l_k1 - p_l_k) / (E_l_k1 - E_l_k);
            if(fr == 0)
                *exit_erg_cm = E_l_k + (r1 - c_k) / p_l_k;
            else
                *exit_erg_cm = E_l_k + (sqrt(MAX(0.0, p_l_k * p_l_k + 2 * fr * (r1 - c_k))) - p_l_k) / fr;

            //// Determine Kalbach-Mann parameters
            if(law_type == 44){
                double R_k = nuc->XSS[LC + 3 * NP + k];
                double R_k1 = nuc->XSS[LC + 3 * NP + k + 1];
                double A_k = nuc->XSS[LC + 4 * NP + k];
                double A_k1 = nuc->XSS[LC + 4 * NP + k + 1];

                KM_R = R_k + (R_k1 - R_k) * (*exit_erg_cm - E_l_k) / (E_l_k1 - E_l_k);
                KM_A = A_k + (A_k1 - A_k) * (*exit_erg_cm - E_l_k) / (E_l_k1 - E_l_k);
            }
        } else{
            printf("unknown interpolation type.MT = %d, LDIS = %d, LDAT = %d ,INTTP = %d, INTT = %d, NP = %d\n",
                   MT, LDIS, LDAT, INTTp, INTT, NP);
            base_warnings++;
            goto End;
        }

        //// interpolate between incident energy bins i and i + 1
        if(nPos_smpl == pos)
            *exit_erg_cm = E_1 + (*exit_erg_cm - E_i_1) * (E_K - E_1) / (E_i_K - E_i_1);
        else
            *exit_erg_cm = E_1 + (*exit_erg_cm - E_i1_1) * (E_K - E_1) / (E_i1_K - E_i1_1);

        ///// sample angular distribution for law 44 & law 61 ////
        if(law_type == 44){ //// Sampled correlated angle from Kalbach-Mann parameters
            double ksi3 = get_rand();
            double ksi4 = get_rand();
            if(ksi3 > KM_R){
                double T = (2 * ksi4 - 1) * sinh(KM_A);
                *exit_mu_cm = log(T + sqrt((T * T) + 1)) / KM_A;
            } else
                *exit_mu_cm = log(ksi4 * exp(KM_A) + (1 - ksi4) * exp(-KM_A)) / KM_A;
            return;
        } else if(law_type == 61){ // sample tabulated angular distribution for law 61
            int LB = ic + NP; // LC + 3*NP + k
            if(INTT != 1 && (nuc->XSS[ib] - r1 < r1 - nuc->XSS[ic]))
                LB = LB + 1;
            int LM = (int) (nuc->XSS[LB]);

            if(LM == 0){ // isotropic
                *exit_mu_cm = 2 * get_rand() - 1;
                return;
            }
            LC = Get_loc_of_DLW(nuc) + abs(LM) - 2;
            // interpolation type and number of points in angular distribution
            int JJ = (int) (nuc->XSS[LC + 1]);
            int NP = (int) (nuc->XSS[LC + 2]);

            //// determine outgoing cosine bin
            double r3 = get_rand();
            double c_k = nuc->XSS[LC + 2 + 2 * NP + 1];
            int k;
            for(k = 1; k <= NP - 1; ++k){
                double temp = nuc->XSS[LC + 2 + 2 * NP + k + 1];
                if(r3 < temp)
                    break;
                c_k = temp;
            }
            k = MIN(k, NP - 1);

            double p_k = nuc->XSS[LC + 2 + NP + k];
            double mu_k = nuc->XSS[LC + 2 + k];
            if(JJ == 1){ //Histogram interpolation
                if(p_k > 0)
                    *exit_mu_cm = mu_k + (r3 - c_k) / p_k;
                else
                    *exit_mu_cm = mu_k;
            } else if(JJ == 2){ // Linear-linear interpolation
                double p_k1 = nuc->XSS[LC + 2 + NP + k + 1];
                double mu_k1 = nuc->XSS[LC + 2 + k + 1];

                double fr = (p_k1 - p_k) / (mu_k1 - mu_k);
                if(fr == 0)
                    *exit_mu_cm = mu_k + (r3 - c_k) / p_k;
                else
                    *exit_mu_cm = mu_k + (sqrt(p_k * p_k + 2 * fr * (r3 - c_k)) - p_k) / fr;
            } else{
                puts("unknown interpolation type.");
                base_warnings++;
                goto End;
            }
        }
    } else if(law_type == 66){ // N-BODY PHASE SPACE DISTRIBUTION
        int NPSX = (int) (nuc->XSS[LDAT]); // number of bodies in phase space
        double Ap = nuc->XSS[LDAT + 1]; // total mass ratio
        double aw = nuc->atom_wgt;  // atomic weight
        double Q = Get_nuc_fis_Q(nuc,
                                 MT); //Nuclides[nuc].XSS[GetLocOfLQR(nuc) + Nuclides[nuc].MTRIndex[mt]-1]; // Q-value
        double E_max = ((Ap - 1) / Ap) * (incident_erg * aw / (aw + 1) + Q);

        double lx = sample_maxwell(1);
        double ly;

        if(NPSX == 3)
            ly = sample_maxwell(1);
        else if(NPSX == 4){
            double temp = get_rand() * get_rand() * get_rand();
            ly = log(temp);
        } else{
            double r1 = get_rand();
            double r2 = get_rand();
            double r3 = get_rand();
            double r4 = get_rand();
            double r5 = get_rand();
            double r6 = get_rand();
            ly = -log(r1 * r2 * r3 * r4) - log(r5) * cos(PI / 2. * r6) * cos(PI / 2. * r6);
        }
        double V = lx / (lx + ly);
        *exit_erg_cm = V * E_max;
        *exit_mu_cm = 2 * get_rand() - 1.;
    } else if(law_type == 67){ // LABORATORY ENERGY-ANGLE LAW
        interpolate_xss_table(nuc, incident_erg, LDAT, &pos, &frac, &NR, &NE);
        LN = LDAT + 2 * NR + NE + 2;

        ///// go to appropriate table for incident energy ////
        int LC[3];   // p2-49    l,  i,   i+1
        LC[1] = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[LN + pos - 1]) - 1;
        LC[2] = 0;

        int ix = 1;
        int ir = 0;
        if(frac != 0){
            LC[2] = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[LN + pos]) - 1;
            ir = 1;
            if(get_rand() <= frac)
                ix = 2;
        }

        double E_i_1[3], E_i_K[3];
        for(int i = 1; i <= 2; i++){
            if(LC[i] == 0)
                continue;
            int le = LC[i];
            int INTMU = (int) (nuc->XSS[le]);
            int NMU = (int) (nuc->XSS[le + 1]);
            int LMU = 0;

            for(int j = 1; j <= NMU - 1; j++){
                if(*exit_mu_cm <= nuc->XSS[le + j + 2]){
                    LMU = j;
                    break;
                }
            }
            if(INTMU == 2){ // lin-lin
                ir = ir + 1;
                if(get_rand() <
                   (*exit_mu_cm - nuc->XSS[le + LMU + 1]) / (nuc->XSS[le + LMU + 2] - nuc->XSS[le + LMU + 1]))
                    LMU = LMU + 1;
            } else if(INTMU != 1){ // lin-lin
                printf("unknown INTMU interpolation type %d.\n", INTMU);
                base_warnings++;
                goto End;
            }

            // sample from tabulated energy distribution
            int lb = Get_loc_of_DLW(nuc) + (int) (nuc->XSS[le + LMU + NMU + 1]);
            int INTEP = (int) (nuc->XSS[lb - 1]);
            int NPEP = (int) (nuc->XSS[lb]);
            E_i_1[i] = nuc->XSS[lb + 1];
            E_i_K[i] = nuc->XSS[lb + NPEP];
            if(i != ix)
                continue;

            ir = ir + 1;
            double ksi4 = get_rand();

            int ic = lb + 2 * NPEP + 1, ib = lb + 3 * NPEP;
            while(ib - ic > 1){
                int ih = (ic + ib) / 2;
                if(ksi4 >= nuc->XSS[ih])
                    ic = ih;
                else
                    ib = ih;
            }

            double E_l_m_k = nuc->XSS[ic - 2 * NPEP];
            double E_l_m_kk = nuc->XSS[ic - 2 * NPEP + 1];
            double p_l_m_k = nuc->XSS[ic - NPEP];
            double p_l_m_kk = nuc->XSS[ic - NPEP + 1];
            double c_l_m_k = nuc->XSS[ic];

            if(INTEP == 1)  //histogram
                *exit_erg_cm = E_l_m_k + (ksi4 - c_l_m_k) / p_l_m_k;
            else if(INTEP == 2){ //lin-lin
                double cal_temp1 = (p_l_m_kk - p_l_m_k) / (E_l_m_kk - E_l_m_k);
                if(cal_temp1 == 0)
                    *exit_erg_cm = E_l_m_k + (ksi4 - c_l_m_k) / p_l_m_k;
                else{
                    double cal_temp2 = sqrt(MAX(0.0, (p_l_m_k * p_l_m_k) + 2 * cal_temp1 * (ksi4 - c_l_m_k)));
                    *exit_erg_cm = E_l_m_k + (cal_temp2 - p_l_m_k) / cal_temp1;
                }
            } else{
                puts("unknown interpolation type.");
                base_warnings++;
                goto End;
            }
        }

        if(frac == 0)
            return;

        double E_1 = E_i_1[1] + frac * (E_i_1[2] - E_i_1[1]);
        double E_K = E_i_K[1] + frac * (E_i_K[2] - E_i_K[1]);
        *exit_erg_cm = E_1 + (*exit_erg_cm - E_i_1[ix]) * (E_K - E_1) / (E_i_K[ix] - E_i_1[ix]);
    } else{
        printf("unknown Law type.Law type = %d.\n", law_type);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }


End:
    if(MT == -1 && law_type != 44 && law_type != 61)
        return;

    if(nuc->LAND[MT] < 0 && law_type != 44 && law_type != 61 && law_type != 66){
        printf("angular distribution is not processed for LAND[MT] = -1. LAW = %d\n", law_type);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }


    if(*exit_erg_cm == 0){ // MT = 91, law = 61, Erg = 0  ??
        *exit_erg_cm = ZERO_ERG;//
    } else if(!(*exit_erg_cm > 0 && *exit_erg_cm < 100)){
        printf("exit energy_cm out of range.\n        nuc=%s, MT=%d, Law=%d, Erg=%9.6E \n",
               nuc->id, MT, law_type, *exit_erg_cm);
        base_warnings++;
        *exit_erg_cm = ZERO_ERG;
    }


    if(!(*exit_mu_cm >= -1.000001 && *exit_mu_cm <= 1.000001)){
        printf("exit mu_cm out of range. nuc=%d, MT=%d, Law=%d, Mu=%f \n", nuc->zaid, MT, law_type, *exit_mu_cm);
        base_warnings++;
        *exit_mu_cm = 2 * get_rand() - 1.;
    }
}