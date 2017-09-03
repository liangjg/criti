//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

int dpplr_brd_nuc_tmp(acedata_t *obj, int nNuc, double nTmp)
{
    int i;
    double a, b, f1, f2;
    //// adjust elastic and total cross-section
    if(fabs(obj->nucs[nNuc]->tmp - nTmp) <= 0.01 * nTmp)
        return 0;   // no adjustment
    b = 500.0 * fabs(nTmp - obj->nucs[nNuc]->tmp) / obj->nucs[nNuc]->atom_wgt;
    for(int j = 1; j <= GetErgGridNum(obj, nNuc); j++) {
        if(obj->nucs[nNuc]->XSS[j] > b)
            break;
        ///////////////calculate f1 //////////////
        f1 = ONE;
        if(obj->nucs[nNuc]->tmp != 0.) {
            a = sqrt(obj->nucs[nNuc]->atom_wgt * obj->nucs[nNuc]->XSS[j] / obj->nucs[nNuc]->tmp);
            if(a >= TWO)
                f1 = ONE + HALF / (a * a);
            i = int(a / 0.04);
            if(a < TWO)
                f1 = (obj->therm_func[i] + (a / 0.04 - i) * (obj->therm_func[i + 1] - obj->therm_func[i])) / a;
        }
        ///////////////calculate f2 //////////////
        f2 = ONE;
        if(nTmp != 0.) {
            a = sqrt(obj->nucs[nNuc]->atom_wgt * obj->nucs[nNuc]->XSS[j] / nTmp);
            if(a >= TWO)
                f2 = ONE + HALF / (a * a);
            i = int(a / .04);
            if(a < TWO)
                f2 = (obj->therm_func[i] + (a / 0.04 - i) * (obj->therm_func[i + 1] - obj->therm_func[i])) / a;
        }
        ///////////////calculate a //////////////
        a = obj->nucs[nNuc]->XSS[j + 3 * GetErgGridNum(obj, nNuc)] * (f2 - f1) / f1;

        ///////////////calculate xs //////////////
        obj->nucs[nNuc]->XSS[j + GetErgGridNum(obj, nNuc)] = obj->nucs[nNuc]->XSS[j + GetErgGridNum(obj, nNuc)] + a;
        obj->nucs[nNuc]->XSS[j + 3 * GetErgGridNum(obj, nNuc)] = obj->nucs[nNuc]->XSS[j + 3 * GetErgGridNum(obj, nNuc)] + a;
    }
    obj->nucs[nNuc]->broaden_tmp = nTmp;
    return 1;
}

void calc_therm_Gfun(acedata_t *obj){
    double b = ONE / sqrt(PI);
    obj->therm_func[0] = TWO * b;
    for(int i = 1; i <= 50; i++){
        double a = i * 0.04;
        obj->therm_func[i] = (a + HALF / a) * erf2_function(a) + b * exp(-a * a);
    }
}

double erf2_function(double a)
{
    // return the value of the error function of a.
    const double WF = 1.1283791670955;

    double ds1[13] = {0.000,        -.0490461212346920,   -.142261205103710,    .0100355821876000,
                      -.000576876469976750, .000027419931252196, -.000001104317550734, .000000038488755420,
                      -.000000001180858253, .000000000032334215, -.000000000000799101, .000000000000017990,
                      -.000000000000000371
    };
    double ds2[22] = { 0.000,  -.0696013466023100,    -.0411013393626210,   .00391449586668960,
                       -.000490639565054900, .000071574790013770,  -.000011530716341312, .000001994670590201,
                       -.000000364266647159, .000000069443726100,  -.000000013712209021, .000000002788389661,
                       -.000000000581416472, .000000000123892049,  -.000000000026906391, .000000000005942614,
                       -.000000000001332386, .000000000000302804,  -.000000000000069666, .000000000000016208,
                       -.000000000000003809, .000000000000000904
    };
    double ds3[23] = { 0.000,   .0715179310202930,      -.0265324343376070,   .00171115397792090,
                       -.000163751663458510, .000019871293500549,  -.000002843712412769, .000000460616130901,
                       -.000000082277530261, .000000015921418724,  -.000000003295071356, .000000000722343973,
                       -.000000000166485584, .000000000040103931,  -.000000000010048164, .000000000002608272,
                       -.000000000000699105, .000000000000192946,  -.000000000000054704, .000000000000015901,
                       -.000000000000004729, .000000000000001432,  -.000000000000000439
    };

    double dErf2 = 0;
    if(fabs(a) <= 8.429370e-8) {
        dErf2 = a * WF;
        return dErf2;
    }
    double b = a * a;
    double b0 = 0;
    double b1 = 0;
    double b2 = 0;
    double c = 0;
    if(b <= ONE) {
        for(int i = 1; i <= 12; i++) {
            b2 = b1;
            b1 = b0;
            b0 = b1 * (4.0 * b - TWO) - b2 + ds1[13 - i];
        }
        dErf2 = a * (ONE + HALF * (b0 - b2));
    } else {
        c = ONE;
        dErf2 = ONE;
        if(a < 0)
            dErf2 = -ONE;
        if(b <= 4.0) {
            c = (16.0 / b - 10.0) / 3.0;
            for(int i = 1; i <= 21; i++) {
                b2 = b1;
                b1 = b0;
                b0 = b1 * c - b2 + ds2[22 - i];
            }
        } else {
            if(b > 32.6987)
                return dErf2;
            c = 16.0 / b - TWO;
            for(int i = 1; i <= 22; i++) {
                b2 = b1;
                b1 = b0;
                b0 = b1 * c - b2 + ds3[23 - i];
            }
        }
        dErf2 = dErf2 - exp(-b) * (ONE + b0 - b2) / (TWO * a);
    }
    return dErf2;
}