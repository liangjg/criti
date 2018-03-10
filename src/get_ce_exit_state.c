//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


void
get_ce_exit_state(particle_status_t *par_status,
                  int MT,
                  bool is_free_gas_col)
{
    double exit_mu_lab;
    double exit_erg_lab;
    nuclide_t *nuc = par_status->nuc;

    get_ce_exist_erg_mu(nuc, MT, par_status->erg_rel, &exit_erg_lab, &exit_mu_lab);

    /* 如果使用了自由气体模型则进一步转换出射方向和能量 */
    if(is_free_gas_col) {
        double atom_tmp, temp1, temp2;

        rotate_dir(exit_mu_lab, par_status->dir_vel, par_status->exit_dir);

        atom_tmp = nuc->atom_wgt / par_status->cell_tmp;
        temp1 = sqrt(exit_erg_lab * atom_tmp);
        for(int i = 0; i < 3; i++)
            par_status->exit_dir[i] = temp1 * par_status->exit_dir[i] + par_status->vel_tgt[i];
        temp2 = SQUARE(par_status->exit_dir[0]) + SQUARE(par_status->exit_dir[1]) + SQUARE(par_status->exit_dir[2]);
        for(int i = 0; i < 3; i++)
            par_status->exit_dir[i] /= sqrt(temp2);
        par_status->exit_erg = temp2 / atom_tmp;
    } else {    /* 转换出射方向 */
        rotate_dir(exit_mu_lab, par_status->dir, par_status->exit_dir);
        par_status->exit_erg = exit_erg_lab;
    }

    if(!(par_status->exit_erg >= ZERO && par_status->exit_erg < 100)) {
        puts("exit energy out of range.");
        par_status->exit_erg = EG0_CUTOFF;
    }
}