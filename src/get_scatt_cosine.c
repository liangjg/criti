//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"

/// 抽样得到出射角余弦
///
/// @param[in] nNuc 碰撞核素Index
/// @param[in] MT 反应mt号
/// @param[in] incident_erg 入射能量
/// return 出射角余弦

double get_scatt_cosine(const nuclide_t *nuc, int MT, double incident_erg){
    double exit_mu_cm = 0;

    //// Isotropic case ////////////
    if(nuc->LAND[MT] == 0){
        exit_mu_cm = 2 * get_rand() - 1;
        return exit_mu_cm;
    }

    double dKsi, dEii, dEi, f1, f2;
    int LOCC, nNE, nLC, min, max, mid;
    int nKsi_int;

    LOCC = GetLocOfAND(nuc) + nuc->LAND[MT];
    nNE = (int) (nuc->XSS[LOCC - 1]);
    min = LOCC;
    max = LOCC + nNE - 1;
    if(incident_erg <= nuc->XSS[LOCC])
        nLC = (int) (nuc->XSS[LOCC + nNE]);
    else if(incident_erg >= nuc->XSS[LOCC + nNE - 1])
        nLC = (int) (nuc->XSS[LOCC + nNE + nNE - 1]);
    else{
        while(max - min > 1){
            mid = (min + max) / 2;
            if(incident_erg >= nuc->XSS[mid])
                min = mid;
            else
                max = mid;
        }
        dEi = nuc->XSS[min];
        dEii = nuc->XSS[max];
        if(get_rand() < (incident_erg - dEi) / (dEii - dEi))
            min = max;
        nLC = (int) (nuc->XSS[min + nNE]);
    }

    if(nLC == 0){ // Isotropic case
        exit_mu_cm = 2 * get_rand() - 1;
        return exit_mu_cm;
    }

    int LOCC2;
    LOCC2 = GetLocOfAND(nuc) + abs(nLC) - 1;
    if(nLC > 0){
        dKsi = 32 * get_rand();
        nKsi_int = (int) (dKsi);
        f1 = nuc->XSS[LOCC2 + nKsi_int];
        f2 = nuc->XSS[LOCC2 + nKsi_int + 1];
        exit_mu_cm = nuc->XSS[LOCC2 + nKsi_int] +
                 (dKsi - nKsi_int) * (nuc->XSS[LOCC2 + nKsi_int + 1] - nuc->XSS[LOCC2 + nKsi_int]);
        goto end;
    }

    int AND_JJ, AND_NP;
    double fa, ca, bb;
    if(nLC < 0){
        nLC = -nLC;
        AND_JJ = (int) (nuc->XSS[LOCC2]); //1=histogram  2=lin-lin
        AND_NP = (int) (nuc->XSS[LOCC2 + 1]);

        min = LOCC2 + 2 * AND_NP + 2;
        max = LOCC2 + 3 * AND_NP + 1;
        dKsi = get_rand();
        while(max - min > 1){
            mid = (min + max) / 2;
            if(dKsi >= nuc->XSS[mid])
                min = mid;
            else
                max = mid;
        }

        fa = nuc->XSS[min - AND_NP];
        ca = nuc->XSS[min - 2 * AND_NP];
        exit_mu_cm = ca + (dKsi - nuc->XSS[min]) / fa;
        if(AND_JJ == 1){
            goto end;
        }
        bb = (nuc->XSS[min - AND_NP + 1] - fa) / (nuc->XSS[min - 2 * AND_NP + 1] - ca);
        if(bb != 0){
            exit_mu_cm = ca + (sqrt(MAX(0.0, fa * fa + 2 * bb * (dKsi - nuc->XSS[min]))) - fa) / bb;
            goto end;
        }
    }

end:
    if(!(exit_mu_cm >= -1.000001 && exit_mu_cm <= 1.000001)){
        printf("exit mu out of range. nuc=%d, MT=%d, Mu_cm=%20.16f\n", nuc->zaid, MT, exit_mu_cm);
        base_warnings++;
        exit_mu_cm = 2 * get_rand() - 1;
    }

    return exit_mu_cm;

}