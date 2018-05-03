//
// Created by xaq on 9/2/17.
//

#include "acedata.h"


double
get_scatt_cosine(const nuclide_t *nuc,
                 RNG_t *RNG,
                 int MT,
                 double incident_erg)
{
    double exit_mu_cm = 0;

    /* 各向同性的情况 */
    if(nuc->LAND[MT] == 0) {
        exit_mu_cm = 2 * get_rand(RNG) - 1;
        return exit_mu_cm;
    }

    /* 各向异性的情况 */
    double Ksi, Eii, Ei;
    int LOCC, NE, LC, min, max, mid;
    int Ksi_int;

    LOCC = Get_loc_of_AND(nuc) + nuc->LAND[MT];
    NE = (int) (nuc->XSS[LOCC - 1]);
    min = LOCC;
    max = LOCC + NE - 1;
    if(incident_erg <= nuc->XSS[min])
        LC = (int) (nuc->XSS[min + NE]);
    else if(incident_erg >= nuc->XSS[max])
        LC = (int) (nuc->XSS[max + NE]);
    else {
        while(max - min > 1) {
            mid = (min + max) / 2;
            if(incident_erg >= nuc->XSS[mid])
                min = mid;
            else
                max = mid;
        }
        Ei = nuc->XSS[min];
        Eii = nuc->XSS[max];
        if(get_rand(RNG) < (incident_erg - Ei) / (Eii - Ei))
            min = max;
        LC = (int) (nuc->XSS[min + NE]);
    }

    if(LC == 0) {    /* 各向同性的情况 */
        exit_mu_cm = 2 * get_rand(RNG) - 1;
        return exit_mu_cm;
    }

    int LOCC2;
    LOCC2 = Get_loc_of_AND(nuc) + abs(LC) - 1;
    if(LC > 0) {    /* 32等概率余弦表 */
        Ksi = 32 * get_rand(RNG);
        Ksi_int = (int) (Ksi);
        exit_mu_cm = nuc->XSS[LOCC2 + Ksi_int] +
                     (Ksi - Ksi_int) * (nuc->XSS[LOCC2 + Ksi_int + 1] - nuc->XSS[LOCC2 + Ksi_int]);
        goto END;
    }

    int AND_JJ, AND_NP;
    double fa, ca, bb;
    /* 角分布表 */
    if(LC < 0) {
        AND_JJ = (int) (nuc->XSS[LOCC2]);
        AND_NP = (int) (nuc->XSS[LOCC2 + 1]);

        min = LOCC2 + 2 * AND_NP + 2;
        max = LOCC2 + 3 * AND_NP + 1;
        Ksi = get_rand(RNG);
        while(max - min > 1) {
            mid = (min + max) / 2;
            if(Ksi >= nuc->XSS[mid])
                min = mid;
            else
                max = mid;
        }

        fa = nuc->XSS[min - AND_NP];
        ca = nuc->XSS[min - 2 * AND_NP];

        if(AND_JJ == 1)
            exit_mu_cm = ca + (Ksi - nuc->XSS[min]) / fa;
        else {
            bb = (nuc->XSS[min - AND_NP + 1] - fa) / (nuc->XSS[min - 2 * AND_NP + 1] - ca);
            if(!EQ_ZERO(bb))
                exit_mu_cm = ca + (sqrt(MAX(0.0, fa * fa + 2 * bb * (Ksi - nuc->XSS[min]))) - fa) / bb;
            else
                exit_mu_cm = ca + (Ksi - nuc->XSS[min]) / fa;
        }
    }

END:
    if(!(exit_mu_cm >= -1.000001 && exit_mu_cm <= 1.000001)) {
        printf("exit mu out of range. nuc=%d, MT=%d, Mu_cm=%20.16f\n", nuc->zaid, MT, exit_mu_cm);
        base_warnings++;
        exit_mu_cm = 2 * get_rand(RNG) - 1;
    }

    return exit_mu_cm;
}