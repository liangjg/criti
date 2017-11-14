//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

/// 将质心系出射角余弦和出射能量转化为实验室系出射角余弦和出射能量
///
/// @param[in] nuc 核素
/// @param[in] MT 反应Mt号
/// @param[in] incident_erg 入射能量
/// @param[in] exit_erg_cm 质心系出射能量
/// @param[in] exit_mu_cm 质心系出射角余弦
/// @param[out] exit_erg_lab 实验室系出射能量
/// @param[out] exit_mu_lab 实验室系出射角余弦
void transform_cm_to_lab(const nuclide_t *nuc, int MT, double incident_erg, double exit_erg_cm, double exit_mu_cm,
                         double *exit_erg_lab, double *exit_mu_lab){
    if(GetEmissNeuNum(nuc, MT) < 0){
        double aw = nuc->atom_wgt;
        *exit_erg_lab = exit_erg_cm + (incident_erg + 2 * exit_mu_cm * (aw + 1) * sqrt(incident_erg * exit_erg_cm)) /
                                      (((1 + aw) * (1 + aw)));
        *exit_mu_lab = exit_mu_cm * sqrt(exit_erg_cm / *exit_erg_lab) + sqrt(incident_erg / *exit_erg_lab) / (aw + 1);
    } else{
        *exit_erg_lab = exit_erg_cm;
        *exit_mu_lab = exit_mu_cm;
    }
}
