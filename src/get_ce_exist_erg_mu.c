//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"


void get_ce_exist_erg_mu(const nuclide_t *nuc, RNG_t *RNG_slave, int MT, double incident_erg, double *exit_erg_lab,
                         double *exit_mu_lab){
    double exit_mu_cm = 0;
    double exit_erg_cm;

    if(nuc->LAND[MT] >= 0)
        exit_mu_cm = get_scatt_cosine(nuc, RNG_slave, MT, incident_erg);

    if(MT == 2){
        double aw = nuc->atom_wgt;
        if(aw >= 1){
            *exit_erg_lab = incident_erg * (ONE + SQUARE(aw) + TWO * aw * exit_mu_cm) / ((ONE + aw) * (ONE + aw));
            *exit_mu_lab = (ONE + aw * exit_mu_cm) / sqrt(ONE + TWO * aw * exit_mu_cm + SQUARE(aw));
        } else{
            *exit_erg_lab = HALF * incident_erg * (ONE + exit_mu_cm);
            *exit_mu_lab = sqrt(HALF + HALF * exit_mu_cm);
        }
    }
    else{
        int LawType, LDAT;
        LawType = get_law_type(nuc, RNG_slave, MT, incident_erg, &LDAT);
        react_by_laws(nuc, RNG_slave, MT, LawType, LDAT, incident_erg, &exit_erg_cm, &exit_mu_cm);
        if(Get_emiss_neu_num(nuc, MT) < 0){
            double aw = nuc->atom_wgt;
            *exit_erg_lab = exit_erg_cm + (incident_erg + 2 * exit_mu_cm * (aw + 1) * sqrt(incident_erg * exit_erg_cm)) /
                                          (((1 + aw) * (1 + aw)));
            *exit_mu_lab = exit_mu_cm * sqrt(exit_erg_cm / *exit_erg_lab) + sqrt(incident_erg / *exit_erg_lab) / (aw + 1);
        } else{
            *exit_erg_lab = exit_erg_cm;
            *exit_mu_lab = exit_mu_cm;
        }
    }

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
        *exit_mu_lab = 2 * get_rand_slave(RNG_slave) - ONE;
    }
}