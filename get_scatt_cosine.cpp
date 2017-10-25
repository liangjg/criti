//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"

double get_scatt_cosine(acedata_t *obj, int nNuc, int nMT, double dIncidErg)
{
    //// Nuc: ��ײ��
    //// MT:  ��Ӧ����
    //// IncidErg: ��������

    double dMu_Cm = 0; // ����ϵ�µ�ɢ�������

    //// Isotropic case ////////////
    if(obj->nucs[nNuc]->LAND[nMT] == 0) {
        dMu_Cm = 2 * get_rand() - 1;
        return dMu_Cm;
    }



    double dKsi, dEii, dEi, f1, f2;
    int LOCC, nNE, nLC, min, max, mid;
    int nKsi_int;

    LOCC = GetLocOfAND(obj, nNuc) + obj->nucs[nNuc]->LAND[nMT];
    nNE = int(obj->nucs[nNuc]->XSS[LOCC - 1]);
    min = LOCC;
    max = LOCC + nNE - 1;
    if(dIncidErg <= obj->nucs[nNuc]->XSS[LOCC])
        nLC = int(obj->nucs[nNuc]->XSS[LOCC + nNE]);
    else if(dIncidErg >= obj->nucs[nNuc]->XSS[LOCC + nNE - 1])
        nLC = int(obj->nucs[nNuc]->XSS[LOCC + nNE + nNE - 1]);
    else {
        while(max - min > 1) {
            mid = (min + max) / 2 ;
            if(dIncidErg >= obj->nucs[nNuc]->XSS[mid])
                min = mid;
            else
                max = mid;
        }
        dEi = obj->nucs[nNuc]->XSS[min];
        dEii = obj->nucs[nNuc]->XSS[max];
        if(get_rand() < (dIncidErg - dEi) / (dEii - dEi))
            min = max;
        nLC = int(obj->nucs[nNuc]->XSS[min + nNE]);
    }

    if(nLC == 0) { // Isotropic case
        dMu_Cm = 2 * get_rand() - 1;
        return dMu_Cm;
    }

    int LOCC2 ;
    LOCC2 = GetLocOfAND(obj, nNuc) + abs(nLC) - 1;
    if(nLC > 0) {
        dKsi = 32 * get_rand();
        nKsi_int = int(dKsi);
        f1 = obj->nucs[nNuc]->XSS[LOCC2 + nKsi_int] ;
        f2 = obj->nucs[nNuc]->XSS[LOCC2 + nKsi_int + 1] ;
        dMu_Cm = obj->nucs[nNuc]->XSS[LOCC2 + nKsi_int] + (dKsi - nKsi_int) * (obj->nucs[nNuc]->XSS[LOCC2 + nKsi_int + 1] - obj->nucs[nNuc]->XSS[LOCC2 + nKsi_int]);
        goto end;
    }

    int AND_JJ, AND_NP;
    double fa, ca, bb;
    if(nLC < 0) {
        nLC = -nLC;
        AND_JJ = int(obj->nucs[nNuc]->XSS[LOCC2]); //1=histogram  2=lin-lin
        AND_NP = int(obj->nucs[nNuc]->XSS[LOCC2 + 1]);

        min = LOCC2 + 2 * AND_NP + 2;
        max = LOCC2 + 3 * AND_NP + 1;
        dKsi = get_rand();
        while(max - min > 1) {
            mid = (min + max) / 2 ;
            if(dKsi >= obj->nucs[nNuc]->XSS[mid])
                min = mid;
            else
                max = mid;
        }

        fa = obj->nucs[nNuc]->XSS[min - AND_NP];
        ca = obj->nucs[nNuc]->XSS[min - 2 * AND_NP];
        dMu_Cm = ca + (dKsi - obj->nucs[nNuc]->XSS[min]) / fa;
        if(AND_JJ == 1) {
            goto end;
        }
        bb = (obj->nucs[nNuc]->XSS[min - AND_NP + 1] - fa) / (obj->nucs[nNuc]->XSS[min - 2 * AND_NP + 1] - ca);
        if(bb != 0) {
            dMu_Cm = ca + (sqrt(MAX(0.0, fa * fa + 2 * bb * (dKsi - obj->nucs[nNuc]->XSS[min]))) - fa) / bb;
            goto end;
        }
    }

    end:
    if(!(dMu_Cm >= -1.000001 && dMu_Cm <= 1.000001)) {
        printf("exit mu out of range. nuc=%d, MT=%d, Mu_cm=%20.16f\n", nNuc, nMT, dMu_Cm);
        base_warnings++;
        dMu_Cm = 2 * get_rand() - 1;
    }

    return dMu_Cm;

}