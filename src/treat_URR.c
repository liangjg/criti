//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "RNG.h"


void treat_URR(nuclide_t *nuc, RNG_t *RNG_slave, nuc_cs_t *cur_nuc_cs, double erg)
{
    double el_factor, fis_factor, C_factor, interp_value, smooth_total, inel_balance, abs_balance;
    int idx, idx_s, idx_e, num_e;

    int num_erg_grid = Get_erg_grid_num(nuc);
    int loc_lunr_table = Get_loc_of_LUNR(nuc);
    int num_prob_table_incident_erg = (int) (nuc->XSS[loc_lunr_table]);
    int loc_incident_erg = loc_lunr_table + 6;
    int length_table = (int) (nuc->XSS[loc_lunr_table + 1]);
    int incident_erg_idx = loc_incident_erg + 1;

    for(;;){
        if(erg < nuc->XSS[incident_erg_idx])
            break;
        incident_erg_idx = incident_erg_idx + 1;
    }

    double ratio =
            (erg - nuc->XSS[incident_erg_idx - 1]) / (nuc->XSS[incident_erg_idx] - nuc->XSS[incident_erg_idx - 1]);
    int erg_idx = incident_erg_idx - loc_incident_erg;
    int loc_prob_table = loc_incident_erg + num_prob_table_incident_erg + (erg_idx - 1) * length_table * 6;
    int nj1 = loc_prob_table;
    int nj2 = loc_prob_table + length_table * 6;

    double rand_num = get_rand_slave(RNG_slave);
    for(;;){
        if(nuc->XSS[nj1] >= rand_num)
            break;
        nj1 = nj1 + 1;
    }
    for(;;){
        if(nuc->XSS[nj2] >= rand_num)
            break;
        nj2 = nj2 + 1;
    }
    if(nj1 - loc_prob_table + 1 > length_table || nj2 - (loc_prob_table + length_table * 6) + 1 > length_table){
        printf("prob table not normed in nuc %s.\n", nuc->id);
        base_warnings++;
        return;
    }

    int distribution_type = (int) (nuc->XSS[loc_lunr_table + 2]);
    if(distribution_type != 5 || ratio <= 0. || ratio >= 1.){
        cur_nuc_cs->abs = nuc->XSS[nj1 + length_table * 4] + ratio * (nuc->XSS[nj2 + length_table * 4] -
                nuc->XSS[nj1 + length_table * 4]);
        cur_nuc_cs->el = nuc->XSS[nj1 + length_table * 2] +
                  ratio * (nuc->XSS[nj2 + length_table * 2] - nuc->XSS[nj1 + length_table * 2]);
        cur_nuc_cs->fis = nuc->XSS[nj1 + length_table * 3] +
                   ratio * (nuc->XSS[nj2 + length_table * 3] - nuc->XSS[nj1 + length_table * 3]);
    } else{
        double g = log(erg / nuc->XSS[incident_erg_idx - 1]) /
                   log(nuc->XSS[incident_erg_idx] / nuc->XSS[incident_erg_idx - 1]);
        int i = length_table * 4;
        cur_nuc_cs->abs = 0;
        if(nuc->XSS[nj1 + i] != 0)
            cur_nuc_cs->abs = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
        i = length_table * 2;
        cur_nuc_cs->el = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
        i = length_table * 3;
        cur_nuc_cs->fis = 0;
        if(nuc->XSS[nj1 + i] != 0)
            cur_nuc_cs->fis = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
    }

    int factor_flag = (int) (nuc->XSS[loc_lunr_table + 5]);
    int inel_competition_flag = (int) (nuc->XSS[loc_lunr_table + 3]);
    int other_abs_flag = (int) (nuc->XSS[loc_lunr_table + 4]);
    if(factor_flag != 0 || inel_competition_flag == 0){
        int interp_pos = cur_nuc_cs->inter_pos;
        el_factor = nuc->XSS[interp_pos + 3 * num_erg_grid] + cur_nuc_cs->inter_frac *
                                                               (nuc->XSS[interp_pos + 3 * num_erg_grid + 1] -
                                                                nuc->XSS[interp_pos + 3 * num_erg_grid]);
        fis_factor = nuc->fis_XSS[interp_pos] +
                     cur_nuc_cs->inter_frac * (nuc->fis_XSS[interp_pos + 1] - nuc->fis_XSS[interp_pos]);
        fis_factor = 0;
        int i = Get_loc_of_FIS(nuc);
        if(i != 0){
            int nInterp_i = cur_nuc_cs->inter_pos - (int) (nuc->XSS[i]) + 1;
            if(nInterp_i >= 1){
                nInterp_i = nInterp_i + i + 1;
                fis_factor = nuc->XSS[nInterp_i] + cur_nuc_cs->inter_frac * (nuc->XSS[nInterp_i + 1] - nuc->XSS[nInterp_i]);
            }
        }
    }

    if(factor_flag != 0 || other_abs_flag == 0.){
        C_factor = 0;
        int i = Get_loc_of_MTR(nuc) - 1;
        for(idx = 1; idx <= Get_non_el_mt_num(nuc); idx++){
            if((int) (nuc->XSS[i + idx]) == 102){
                idx_s = Get_loc_of_SIG(nuc) +
                        (int) (nuc->XSS[Get_loc_of_LSIG(nuc) + idx - 1]);
                idx_e = (int) (nuc->XSS[idx_s - 1]);
                num_e = (int) (nuc->XSS[idx_s]);
                int ic = cur_nuc_cs->inter_pos + 1 - idx_e;
                if(ic >= 1 && ic <= num_e){
                    if(ic == num_e)
                        C_factor = nuc->XSS[idx_s + num_e];
                    else
                        C_factor = nuc->XSS[idx_s + ic] +
                                   cur_nuc_cs->inter_frac * (nuc->XSS[idx_s + ic + 1] - nuc->XSS[idx_s + ic]);
                }
                break;
            }
        }


        if(factor_flag != 0){
            cur_nuc_cs->el = cur_nuc_cs->el * el_factor;
            cur_nuc_cs->fis = cur_nuc_cs->fis * fis_factor;
            cur_nuc_cs->abs = cur_nuc_cs->abs * C_factor;
        }
    }

    inel_balance = 0;
    abs_balance = 0;


    if(inel_competition_flag == 0 || other_abs_flag == 0){
        int ii = cur_nuc_cs->inter_pos;
        interp_value = nuc->XSS[ii + 2 * num_erg_grid] +
                       cur_nuc_cs->inter_frac * (nuc->XSS[ii + 2 * num_erg_grid + 1] - nuc->XSS[ii + 2 * num_erg_grid]);
    }

    if(inel_competition_flag > 0){
        int i = Get_loc_of_MTR(nuc) - 1;
        for(idx = 1; idx <= Get_non_el_mt_num(nuc); ++idx){
            if((int) (nuc->XSS[i + idx]) == inel_competition_flag)
                break;
        }
        if(idx > Get_non_el_mt_num(nuc)){
            puts("total inelastic MT is not in table.");
            base_warnings++;
        }

        idx_s = Get_loc_of_SIG(nuc) + (int) (nuc->XSS[Get_loc_of_LSIG(nuc) + idx - 1]);
        idx_e = (int) (nuc->XSS[idx_s - 1]);
        num_e = (int) (nuc->XSS[idx_s]);
        int idx_c = cur_nuc_cs->inter_pos + 1 - idx_e;
        if(idx_c >= 1 && idx_c <= num_e){
            if(idx_c == num_e)
                inel_balance = nuc->XSS[idx_s + num_e];
            else
                inel_balance = nuc->XSS[idx_s + idx_c] +
                               cur_nuc_cs->inter_frac * (nuc->XSS[idx_s + idx_c + 1] - nuc->XSS[idx_s + idx_c]);
        }
    }
    else if(inel_competition_flag == 0){

        int nIdx_c = cur_nuc_cs->inter_pos;
        smooth_total = nuc->XSS[nIdx_c + num_erg_grid] +
                       cur_nuc_cs->inter_frac * (nuc->XSS[nIdx_c + num_erg_grid + 1] - nuc->XSS[nIdx_c + num_erg_grid]);
        inel_balance = smooth_total - el_factor - fis_factor - interp_value;
    }

    if(other_abs_flag > 0){
        int i = Get_loc_of_MTR(nuc) - 1;
        for(idx = 1; idx <= Get_non_el_mt_num(nuc); ++idx){
            if((int) (nuc->XSS[i + idx]) == other_abs_flag)
                break;
        }
        if(idx > Get_non_el_mt_num(nuc)){
            puts("other absorption mt not in table.");
            base_warnings++;
        }

        idx_s = Get_loc_of_SIG(nuc) + (int) (nuc->XSS[Get_loc_of_LSIG(nuc) + idx - 1]);
        idx_e = (int) (nuc->XSS[idx_s - 1]);
        num_e = (int) (nuc->XSS[idx_s]);
        int idx_c = cur_nuc_cs->inter_pos + 1 - (int) (nuc->XSS[idx_s - 1]);
        if(idx_c >= 1 && idx_c <= num_e){
            if(idx_c == num_e)
                abs_balance = nuc->XSS[idx_s + num_e];
            else
                abs_balance = nuc->XSS[idx_s + idx_c] +
                              cur_nuc_cs->inter_frac * (nuc->XSS[idx_s + idx_c + 1] - nuc->XSS[idx_s + idx_c]);
        }
    } else if(other_abs_flag == 0)
        abs_balance = interp_value - C_factor;

    cur_nuc_cs->abs = cur_nuc_cs->abs + abs_balance;
    if(cur_nuc_cs->abs < 0){
        int NE = Get_erg_grid_num(nuc);
        cur_nuc_cs->abs = intplt_by_pos_fr(nuc->XSS, cur_nuc_cs->inter_pos + 2 * NE, cur_nuc_cs->inter_frac);
    }

    cur_nuc_cs->inel = inel_balance;
    cur_nuc_cs->tot = cur_nuc_cs->abs + cur_nuc_cs->el + cur_nuc_cs->inel + cur_nuc_cs->fis;

}