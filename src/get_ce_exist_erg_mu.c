//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"

/// 计算连续能量Ace截面情形下的出射角余弦和出射能量
///
/// @param[in] nuc 碰撞核素
/// @param[in] MT 反应mt号
/// @param[in] incident_erg 入射能量
/// @param[out] exit_erg_lab 实验室系出射能量
/// @param[out] exit_mu_lab 实验室系出射角余弦
void get_ce_exist_erg_mu(const nuclide_t *nuc, int MT, double incident_erg, double *exit_erg_lab, double *exit_mu_lab){
    double exit_mu_cm = 0;
    double exit_erg_cm;

    if(nuc->LAND[MT] >= 0)
        exit_mu_cm = get_scatt_cosine(nuc, MT, incident_erg);

    /// 弹性散射情形
    if(MT == 2){
        double aw = nuc->atom_wgt;
        if(aw >= 1){
            *exit_erg_lab = incident_erg * (ONE + SQUARE(aw) + TWO * aw * exit_mu_cm) / ((ONE + aw) * (ONE + aw));
            *exit_mu_lab = (ONE + aw * exit_mu_cm) / sqrt(ONE + TWO * aw * exit_mu_cm + SQUARE(aw));
        } else{
            *exit_erg_lab = HALF * incident_erg * (ONE + exit_mu_cm);
            *exit_mu_lab = sqrt(HALF + HALF * exit_mu_cm);
        }
    }/// 非弹性散射情形
    else{
        int LawType, LDAT;
        ///   抽样Law
        LawType = get_law_type(nuc, MT, incident_erg, &LDAT);
        ///   根据Law确定质心系出射角余弦和出射能量
        react_by_laws(nuc, MT, LawType, LDAT, incident_erg, &exit_erg_cm, &exit_mu_cm);
        ///   将质心系出射角余弦和出射能量转化为实验室系出射角余弦和出射能量
        transform_cm_to_lab(nuc, MT, incident_erg, exit_erg_cm, exit_mu_cm, exit_erg_lab, exit_mu_lab);
    }

    /////////////////  check exit Erg and Mu in lab /////////////
    if(*exit_erg_lab == 0)  // occasionally with MT = 2
        *exit_erg_lab = ZERO_ERG;
    else if(!(*exit_erg_lab > 0 && *exit_erg_lab < 100)){
        printf("exit erg_lab out of range. nuc=%d, MT=%d, Erg=%9.6E\n", nuc->zaid, MT, *exit_erg_lab);
        base_warnings++;
        *exit_erg_lab = ZERO_ERG;
    }

    if(!(*exit_mu_lab >= -1.000001 && *exit_mu_lab <= 1.000001)){
        printf("exit mu_lab out of range. nuc=%d, MT=%d, Mu=%f\n", nuc->zaid, MT, *exit_mu_lab);
        base_warnings++;
        *exit_mu_lab = 2 * get_rand() - 1.;
    }
}