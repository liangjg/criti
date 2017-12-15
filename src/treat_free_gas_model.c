//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"


void treat_free_gas_model(particle_state_t *par_state, double nuc_wgt){
    double dAtomTmp;
    double r1, z2, s, z, c, x2;
    double dYcn;
    int nIterCount = 0;

    /////// sample the velocity of the target nucleus.///////////
    dAtomTmp = nuc_wgt / par_state->cell_tmp; //?
    dYcn = sqrt(par_state->erg * dAtomTmp); //Temperature-normalized neutron velocity.
    do{
        nIterCount++;
        if(nIterCount >= MAX_ITER){
            puts("Waring: too many samples of Free gas model.");
            exit(0);
        }


        if(get_rand() * (dYcn + 1.12837917) > dYcn){
            r1 = get_rand();
            z2 = -log(r1 * get_rand());
        } else{
            do{
                double ksi1 = get_rand();
                double ksi2 = get_rand();
                r1 = ksi1 * ksi1;
                s = r1 + ksi2 * ksi2;
            } while(s > 1);
            z2 = -r1 * log(s) / s - log(get_rand());
        }
        z = sqrt(z2);
        c = 2 * get_rand() - 1.;
        x2 = dYcn * dYcn + z2 - 2 * dYcn * z * c;
    } while(pow(get_rand() * (dYcn + z), 2) > x2);

    rotate_dir(c, par_state->dir, par_state->vel_tgt);

    /////////// calculate functions of the target velocity.////////
    for(int i = 0; i < 3; ++i){
        par_state->vel_tgt[i] = z * par_state->vel_tgt[i];
        par_state->dir_vel[i] = dYcn * par_state->dir[i] - par_state->vel_tgt[i];
    }

    //    CDGlobeFun::Normalize3Array(par_state->dir_vel);
    double length = ONE / sqrt(SQUARE(par_state->dir_vel[0]) +
                                      SQUARE(par_state->dir_vel[1]) + SQUARE(par_state->dir_vel[2]));
    par_state->dir_vel[0] *= length;
    par_state->dir_vel[1] *= length;
    par_state->dir_vel[2] *= length;

    par_state->erg_rel = x2 / dAtomTmp;

    if(!(par_state->erg_rel > 0 && par_state->erg_rel < 100))
        par_state->erg_rel = EG0_CUTOFF;
}