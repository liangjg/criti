//
// Created by xaq on 2018/1/29.
//

#include "surface.h"


#define RETURN(sense)  \
    do{  \
        if(GT_ZERO(sense)) return 1;  \
        else return -1;  \
    } while(0)

int
calc_surf_sense_P(const double *paras,
                  const double *pos,
                  const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = paras[0] * x + paras[1] * y + paras[2] * z - paras[3];

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        for(int i = 0; i < 3; i++)
            sense += dir[i] * paras[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_PX(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = x - paras[0];

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {ONE, ZERO, ZERO};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_PY(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = y - paras[0];

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {ZERO, ONE, ZERO};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_PZ(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = z - paras[0];

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {ZERO, ZERO, ONE};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_SO(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x) + SQUARE(y) + SQUARE(z) - SQUARE(paras[0]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x, y, z};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_S(const double *paras,
                  const double *pos,
                  const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x - paras[0]) + SQUARE(y - paras[1]) + SQUARE(z - paras[2]) - SQUARE(paras[3]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x - paras[0], y - paras[1], z - paras[2]};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_SX(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x - paras[0]) + SQUARE(y) + SQUARE(z) - SQUARE(paras[1]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x - paras[0], y, z};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_SY(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x) + SQUARE(y - paras[0]) + SQUARE(z) - SQUARE(paras[1]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x, y - paras[0], z};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_SZ(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x) + SQUARE(y) + SQUARE(z - paras[0]) - SQUARE(paras[1]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x, y, z - paras[0]};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_C_X(const double *paras,
                    const double *pos,
                    const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(y - paras[0]) + SQUARE(z - paras[1]) - SQUARE(paras[2]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {ZERO, y - paras[0], z - paras[1]};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_C_Y(const double *paras,
                    const double *pos,
                    const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x - paras[0]) + SQUARE(z - paras[1]) - SQUARE(paras[2]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x - paras[0], ZERO, z - paras[1]};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_C_Z(const double *paras,
                    const double *pos,
                    const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x - paras[0]) + SQUARE(y - paras[1]) - SQUARE(paras[2]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x - paras[0], y - paras[1], ZERO};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_CX(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(y) + SQUARE(z) - SQUARE(paras[0]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {ZERO, y, z};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_CY(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x) + SQUARE(z) - SQUARE(paras[0]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x, ZERO, z};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}

int
calc_surf_sense_CZ(const double *paras,
                   const double *pos,
                   const double *dir)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];
    double sense = SQUARE(x) + SQUARE(y) - SQUARE(paras[0]);

    if(fabs(sense) < EPSILON) {
        sense = ZERO;
        double norm_vec[3] = {x, y, ZERO};
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    RETURN(sense);
}
