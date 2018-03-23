//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "neutron_transport.h"


void
treat_sab_colli_type(const nuclide_t *sab_nuc,
                     particle_status_t *par_status,
                     RNG_t *RNG_slave)
{
    int sab_n_el, sab_n_inel;
    int min, max;
    double sab_k_el, sab_k_inel;
    double exit_mu_lab = ZERO;
    nuc_xs_t *cur_nuc_cs = par_status->nuc_xs;
    double SIG_sab_el = cur_nuc_cs->el;
    double SIG_sab_inel = cur_nuc_cs->inel;
    double incident_erg = par_status->erg;

    if(Get_loc_of_sab_el_erg(sab_nuc) != 0 && get_rand_slave(RNG_slave) * (SIG_sab_el + SIG_sab_inel) > SIG_sab_inel) {
        int NE_el, LN, N_el_mu, Loc, mode_el;

        par_status->exit_erg = incident_erg;
        NE_el = (int) (sab_nuc->XSS[Get_loc_of_sab_el_erg(sab_nuc)]);
        min = Get_loc_of_sab_el_erg(sab_nuc) + 1;
        max = Get_loc_of_sab_el_erg(sab_nuc) + NE_el;
        sab_n_el = get_intplt_pos_fr(sab_nuc->XSS, incident_erg, min, max, &sab_k_el);
        sab_n_el = sab_n_el - Get_loc_of_sab_el_erg(sab_nuc);
        LN = Get_loc_of_sab_el_mu(sab_nuc) + (sab_n_el - 1) * (abs(Get_sab_el_dim_para(sab_nuc)) + 1);
        N_el_mu = Get_sab_el_dim_para(sab_nuc) + 1;
        mode_el = Get_sab_el_mode(sab_nuc);
        if(mode_el == 2) {
            if(get_rand_slave(RNG_slave) <= sab_k_el)
                LN = LN + N_el_mu;
            double ksi = (N_el_mu - 1) * get_rand_slave(RNG_slave) + 1;
            Loc = (int) (LN + ksi);
            exit_mu_lab = sab_nuc->XSS[Loc] + (sab_nuc->XSS[Loc - 1] - sab_nuc->XSS[Loc]) * (ksi - (int) (ksi));
        } else if(mode_el == 3) {
            Loc = LN + (int) (N_el_mu * get_rand_slave(RNG_slave));
            exit_mu_lab = sab_nuc->XSS[Loc] + sab_k_el * (sab_nuc->XSS[Loc + N_el_mu] - sab_nuc->XSS[Loc]);
        } else if(mode_el == 4) {
            double pi;

            min = Get_loc_of_sab_el_xs(sab_nuc);
            max = Get_loc_of_sab_el_xs(sab_nuc) + sab_n_el;
            pi = sab_nuc->XSS[max] * get_rand_slave(RNG_slave);
            Loc = get_intplt_pos(sab_nuc->XSS, pi, min, max);
            Loc = Loc + 1 - (int) (sab_nuc->XSS[Get_loc_of_sab_el_erg(sab_nuc)]);
            exit_mu_lab = 1 - 2.0 * sab_nuc->XSS[Loc] / incident_erg;
        } else {
            printf("incorrect elastic scattering mode(%d) in sab collision.\n", mode_el);
            base_warnings++;
        }
    } else {
        int NE_inel, LE;

        NE_inel = (int) (sab_nuc->XSS[Get_loc_of_sab_inel_erg(sab_nuc)] + 0.5);
        min = Get_loc_of_sab_inel_erg(sab_nuc) + 1;
        max = Get_loc_of_sab_inel_erg(sab_nuc) + NE_inel;
        sab_n_inel = get_intplt_pos_fr(sab_nuc->XSS, incident_erg, min, max, &sab_k_inel);
        sab_n_inel = sab_n_inel - Get_loc_of_sab_inel_erg(sab_nuc);
        if(Get_sab_sec_erg_mode(sab_nuc) == 0)
            LE = (int) (get_rand_slave(RNG_slave) * Get_sab_inel_eout_num(sab_nuc));
        else {
            double rr = get_rand_slave(RNG_slave) * (Get_sab_inel_eout_num(sab_nuc) - 3);
            if(rr >= 1.0)
                LE = (int) (rr + 1);
            else {
                if(rr >= 0.5) {
                    LE = Get_sab_inel_eout_num(sab_nuc) - 2;
                    if(rr < 0.6)
                        LE = LE + 1;
                } else {
                    LE = 1;
                    if(rr < 0.1)
                        LE = 0;
                }
            }
        }
        int N_inel_mu, LN_offset, LN, mode_inel;

        N_inel_mu = Get_sab_inel_dim_para(sab_nuc) + 1;
        LN_offset = Get_sab_inel_eout_num(sab_nuc) * (N_inel_mu + 1);
        LN = (sab_n_inel - 1) * LN_offset + Get_loc_of_sab_inel_erg_mu(sab_nuc) + LE * (N_inel_mu + 1);
        par_status->exit_erg = sab_nuc->XSS[LN] + sab_k_inel * (sab_nuc->XSS[LN + LN_offset] - sab_nuc->XSS[LN]);
        if(par_status->exit_erg <= 0) {
            puts("exit energy in sab collision is out of range.");
            base_warnings++;
            par_status->exit_erg = EG0_CUTOFF;
        } else if(par_status->exit_erg <= 1.0E-11) {
            if(par_status->exit_erg > EG0_CUTOFF)
                par_status->exit_erg = 1.0E-11;
        }

        LN = LN + 1;
        mode_inel = Get_sab_inel_mode(sab_nuc);
        if(mode_inel == 2) {
            if(get_rand_slave(RNG_slave) <= sab_k_inel)
                LN = LN + LN_offset;
            double ksi = (N_inel_mu - 1) * get_rand_slave(RNG_slave) + 1;
            int Loc = (int) (LN + ksi);
            exit_mu_lab = sab_nuc->XSS[Loc] + (sab_nuc->XSS[Loc - 1] - sab_nuc->XSS[Loc]) * (ksi - (int) (ksi));
        } else if(mode_inel == 3) {
            int Loc = LN + (int) (N_inel_mu * get_rand_slave(RNG_slave));
            exit_mu_lab = sab_nuc->XSS[Loc] + sab_k_inel * (sab_nuc->XSS[Loc + LN_offset] - sab_nuc->XSS[Loc]);
        } else {
            printf("incorrect inelastic scattering mode(%d) in sab collision.\n", mode_inel);
            base_warnings++;
        }
    }

    exit_mu_lab = MIN(ONE, exit_mu_lab);
    exit_mu_lab = MAX(-ONE, exit_mu_lab);

    rotate_dir(exit_mu_lab, par_status->dir, par_status->exit_dir, RNG_slave);
}