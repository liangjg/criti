//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"


extern RNG_t RNG_slave;

void treat_free_gas_model(particle_state_t *par_state, double nuc_wgt){
    double atom_tmp;
    double r1, z2, s, z, c, x2;
    double Ycn;
    int iter_count = 0;

    /////// sample the velocity of the target nucleus.///////////
    atom_tmp = nuc_wgt / par_state->cell_tmp; //?
    Ycn = sqrt(par_state->erg * atom_tmp); //Temperature-normalized neutron velocity.
    do{
        if((iter_count++) >= MAX_ITER){
            puts("Waring: too many samples of Free gas model.");
            release_resource();
            exit(0);
        }


        if(get_rand(&RNG_slave) * (Ycn + 1.12837917) > Ycn){
            r1 = get_rand(&RNG_slave);
            z2 = -log(r1 * get_rand(&RNG_slave));
        } else{
            do{
                double ksi1 = get_rand(&RNG_slave);
                double ksi2 = get_rand(&RNG_slave);
                r1 = ksi1 * ksi1;
                s = r1 + ksi2 * ksi2;
            } while(s > 1);
            z2 = -r1 * log(s) / s - log(get_rand(&RNG_slave));
        }
        z = sqrt(z2);
        c = 2 * get_rand(&RNG_slave) - 1.;
        x2 = Ycn * Ycn + z2 - 2 * Ycn * z * c;
    } while(pow(get_rand(&RNG_slave) * (Ycn + z), 2) > x2);

    rotate_dir(c, par_state->dir, par_state->vel_tgt);

    /////////// calculate functions of the target velocity.////////
    for(int i = 0; i < 3; ++i){
        par_state->vel_tgt[i] = z * par_state->vel_tgt[i];
        par_state->dir_vel[i] = Ycn * par_state->dir[i] - par_state->vel_tgt[i];
    }

    //    CDGlobeFun::Normalize3Array(par_state->dir_vel);
    double length = ONE / sqrt(SQUARE(par_state->dir_vel[0]) +
                                      SQUARE(par_state->dir_vel[1]) + SQUARE(par_state->dir_vel[2]));
    par_state->dir_vel[0] *= length;
    par_state->dir_vel[1] *= length;
    par_state->dir_vel[2] *= length;

    par_state->erg_rel = x2 / atom_tmp;

    if(!(par_state->erg_rel > 0 && par_state->erg_rel < 100))
        par_state->erg_rel = EG0_CUTOFF;
}