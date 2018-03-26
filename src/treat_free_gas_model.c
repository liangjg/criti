//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"


void
treat_free_gas_model(particle_status_t *par_status,
                     RNG_t *RNG,
                     double nuc_wgt)
{
    double atom_tmp;
    double r1, z2, s, z, c, x2;
    double Ycn;
    int iter_count = 0;
    int i;

    atom_tmp = nuc_wgt / par_status->cell_tmp;
    Ycn = sqrt(par_status->erg * atom_tmp);
    do {
        if((iter_count++) >= MAX_ITER) {
            puts("Waring: too many samples of Free gas model.");
            base_warnings++;
        }

        if(get_rand_slave(RNG) * (Ycn + 1.12837917) > Ycn) {
            r1 = get_rand_slave(RNG);
            z2 = -log(r1 * get_rand_slave(RNG));
        } else {
            do {
                double ksi1 = get_rand_slave(RNG);
                double ksi2 = get_rand_slave(RNG);
                r1 = ksi1 * ksi1;
                s = r1 + ksi2 * ksi2;
            } while(s > 1);
            z2 = -r1 * log(s) / s - log(get_rand_slave(RNG));
        }
        z = sqrt(z2);
        c = 2 * get_rand_slave(RNG) - 1.;
        x2 = Ycn * Ycn + z2 - 2 * Ycn * z * c;
    } while(pow(get_rand_slave(RNG) * (Ycn + z), 2) > x2);

    rotate_dir(c, par_status->dir, par_status->vel_tgt, RNG);

    for(i = 0; i < 3; ++i) {
        par_status->vel_tgt[i] = z * par_status->vel_tgt[i];
        par_status->dir_vel[i] = Ycn * par_status->dir[i] - par_status->vel_tgt[i];
    }

    double length = ONE / sqrt(SQUARE(par_status->dir_vel[0]) +
                               SQUARE(par_status->dir_vel[1]) + SQUARE(par_status->dir_vel[2]));
    par_status->dir_vel[0] *= length;
    par_status->dir_vel[1] *= length;
    par_status->dir_vel[2] *= length;

    par_status->erg_rel = x2 / atom_tmp;

    if(!(par_status->erg_rel > 0 && par_status->erg_rel < 100))
        par_status->erg_rel = EG0_CUTOFF;
}