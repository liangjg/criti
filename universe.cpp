//
// Created by xaq on 10/7/17.
//

#include "universe.h"


/* -------------------------- private prototypes ---------------------------- */
int _find_lat_index_rect(universe_t *obj, const double pos[3], const double dir[3]);

int _find_lat_index_hex(universe_t *obj, const double pos[3], const double dir[3]);

double _calc_dist_to_lat_rect(universe_t *obj, const double pos[3], const double dir[3], int *which_surf);

double _calc_dist_to_lat_hex(universe_t *obj, const double pos[3], const double dir[3], int *which_surf);

/* ----------------------------- API implementation ------------------------- */
universe_t *univ_init() {
    universe_t *_new_univ = new universe_t;
    _new_univ->contain_cell_num = 0;
    _new_univ->is_lattice = false;
    _new_univ->is_moved = false;
    _new_univ->is_rotated = false;
    _new_univ->cos_sita = 0.0;
    _new_univ->sin_sita = 0.0;
    _new_univ->sita = 0.0;
    _new_univ->fill_cells = nullptr;
    _new_univ->fill_lat_univese = nullptr;
    _new_univ->filled_lat_num = 0;
    _new_univ->height = 0.0;
    _new_univ->lattice_type = 0;
    return _new_univ;
}

void trans_univ_coord(universe_t *obj, double pos[3], double dir[3]) {
    if(obj->is_moved)
        for(int i = 0; i < 3; i++)
            pos[i] -= obj->origin[i];

    if(obj->is_rotated){
        double pos_temp[3];
        double dir_temp[3];
        for(int i = 0; i < 3; i++){
            pos_temp[i] = pos[i];
            dir_temp[i] = dir[i];
        }

        for(int i = 0; i < 3; i++){
            pos[i] = 0;
            dir[i] = 0;
            for(int j = 0; j < 3; j++){
                pos[i] += obj->rotation[i][j] * pos_temp[j];
                dir[i] += obj->rotation[i][j] * dir_temp[j];
            }
        }
    }
}

void trans_univ_dir(universe_t *obj, double dir[3]) {
    if(obj->is_rotated){
        double dir_temp[3];
        for(int i = 0; i < 3; i++)
            dir_temp[i] = dir[i];

        for(int i = 0; i < 3; i++){
            dir[i] = 0;
            for(int j = 0; j < 3; j++)
                dir[i] += obj->rotation[i][j] * dir_temp[j];
        }
    }
}

int find_lat_index(universe_t *obj, const double pos[3], const double dir[3]) {
    int lat_index = -1;
    if(obj->lattice_type == 1)
        _find_lat_index_rect(obj, pos, dir);
    else if(obj->lattice_type == 2)
        _find_lat_index_hex(obj, pos, dir);
    else puts("unknown lattice type.");

    if(lat_index <= 0 || lat_index > obj->filled_lat_num){
        puts("failed to locate lattice index.");
        lat_index = -1;
        warnings++;
    }
    return lat_index;
}

void move_to_origin_lat(universe_t *obj, int lat_index, double pos[3]) {
    if(obj->lattice_type == 1){
        int xyz[3];
        xyz[2] = (lat_index - 1) / (obj->scope[0] * obj->scope[1]);
        xyz[1] = (lat_index - 1 - obj->scope[0] * obj->scope[1] * xyz[2]) / obj->scope[0];
        xyz[0] = lat_index - 1 - obj->scope[0] * obj->scope[1] * xyz[2] - obj->scope[0] * xyz[1];

        for(int i = 0; i < 3; i++)
            if(obj->scope[i] != 1)
                pos[i] -= obj->pitch[i] * xyz[i];
    } else if(obj->lattice_type == 2){
        double len1 = obj->pitch[0];
        int i2 = (lat_index - 1) / obj->scope[0];
        int i1 = lat_index - 1 - i2 * obj->scope[0];

        pos[0] -= (i1 * len1 + i2 * 0.5 * len1);
        pos[1] -= i2 * obj->height;
    } else {
        puts("unknown lattice type.");
        warnings++;
    }
}

double calc_dist_to_lat(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {
    double distance = -1.0;
    if(obj->lattice_type == 1)
        distance = _calc_dist_to_lat_rect(obj, pos, dir, which_surf);
    else if(obj->lattice_type == 2)
        distance = _calc_dist_to_lat_hex(obj, pos, dir, which_surf);
    else {
        puts("unknown lattice type.");
        warnings++;
    }

    return distance;
}

/* ------------------------ private API implementation ---------------------- */
double _calc_dist_to_lat_rect(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {
    double distance = 1.0E+24;
    double distance_temp;
    int surf_order;

    for(int i = 0; i < 3; i++){
        if(obj->scope[i] == 1 || pos[i] < EPSILON || pos[i] > -EPSILON)
            continue;

        if(pos[i] > EPSILON){
            distance_temp = (obj->pitch[i] - pos[i]) / dir[i];
            surf_order = 2 * i + 2;
        } else {
            distance_temp = -pos[i] / dir[i];
            surf_order = 2 * i + 1;
        }

        if(distance_temp < distance){
            distance = distance_temp;
            *which_surf = surf_order;
        }
    }

    return distance;
}

double _calc_dist_to_lat_hex(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {

    double pos1 = pos[0], pos2 = pos[1];
    double dir1 = dir[0], dir2 = dir[1];
    double L1 = obj->pitch[0];    //  L2 = Pitch[1];
    double D2 = 0.5 * L1 * obj->cos_sita + obj->height * obj->sin_sita;


    double distance_temp[7];

    ///// FA & CD
    if(dir1 > 0){
        distance_temp[1] = -1;
        distance_temp[4] = (0.5 * L1 - pos1) / dir1;
    } else {
        distance_temp[1] = (-0.5 * L1 - pos1) / dir1;
        distance_temp[4] = -1;
    }

    ///// AB & DE
    double t1 = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
    double t2 = pos1 * obj->cos_sita + pos2 * obj->sin_sita;
    if(t1 > 0){ //// DE
        distance_temp[2] = -1;
        distance_temp[5] = (0.5 * D2 - t2) / t1;
    } else { //// AB
        distance_temp[2] = (-0.5 * D2 - t2) / t1;
        distance_temp[5] = -1;
    }

    ///// BC & EF
    double t3 = dir1 * obj->cos_sita - dir2 * obj->sin_sita;
    double t4 = pos1 * obj->cos_sita - pos2 * obj->sin_sita;
    if(t3 > 0){ //// BC
        distance_temp[3] = (0.5 * D2 - t4) / t3;
        distance_temp[6] = -1;
    } else { //// EF
        distance_temp[3] = -1;
        distance_temp[6] = (-0.5 * D2 - t4) / t3;
    }

    /////////////////////////////// select minimum distance to lattice boundary //////////////////////
    double distance = 1.0E+24;
    for(int i = 1; i <= 6; ++i){
        if(distance_temp[i] > 0){
            if(distance_temp[i] < distance){
                distance = distance_temp[i];
                *which_surf = i;
            }
        }
    }

    return distance;
}

int _find_lat_index_rect(universe_t *obj, const double pos[3], const double dir[3]) {
    int lat_index = 0;
    int xyz[3];
    for(int i = 0; i < 3; ++i){
        if(obj->scope[i] == 1)
            xyz[i] = 0;
        else {
            double temp1 = pos[i] / obj->pitch[i];
            int temp2 = int(temp1 + 0.5);

            if(fabs(temp1 - temp2) < EPSILON){
                if(dir[i] >= 0)
                    xyz[i] = temp2;
                else
                    xyz[i] = temp2 - 1;
            } else
                xyz[i] = int(temp1);
        }
    }
    lat_index = 1 + xyz[0] + obj->scope[0] * xyz[1] + obj->scope[0] * obj->scope[1] * xyz[2];

    return lat_index;
}

int _find_lat_index_hex(universe_t *obj, const double pos[3], const double dir[3]) {
    //////////////////////////////// calculate lattice parameters ///////////////////////////////////
    double pos1 = pos[0], pos2 = pos[1];             //// point position in 2D rectangular coordinates
    double dir1 = dir[0], dir2 = dir[1];             //// point direction in 2D rectangular coordinates
    double len1 = obj->pitch[0];                      //// dLen2 = Pitch[1];
    double dis2 = 0.5 * len1 * obj->cos_sita + obj->height * obj->sin_sita; ////  distance between AB and DE


    ////////////////////////////////// find parallelogram index  ////////////////////////////////////
    ////////////////// AM = k1*b1 + k2*b2, where b1=(L1,0) and b2 = (L1/2,height)  ////////////////////
    double dK2 = pos2 / obj->height;
    double dK1 = (pos1 - dK2 * 0.5 * len1) / len1;
    int i1 = int(floor(dK1)); //// note: ik1,ik2 may be start from -1
    int i2 = int(floor(dK2));
    ///////////////////// offset r1 r2 to parallelogram of index 0 //////////////////////////
    pos1 = pos1 - (i1 * len1 + i2 * 0.5 * len1);
    pos2 = pos2 - i2 * obj->height;



    ///////////////////////////////// find hexagon index ///////////////////////////////////////////
    double sense1 = pos1 - len1;
    if(fabs(sense1) < EPSILON)
        sense1 = dir1 > 0 ? 1 : -1;
    if(sense1 > 0){
        double sense2 = (pos1 - len1) * obj->cos_sita + pos2 * obj->sin_sita - 0.5 * dis2;
        if(fabs(sense2) < EPSILON){
            double sense_dir = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
            sense2 = sense_dir > 0 ? 1 : -1;
        }
        i1 = i1 + 1;
        if(sense2 > 0)
            i2 = i2 + 1;
    } else {
        double sense3 = pos1 - 0.5 * len1;
        if(fabs(sense3) < EPSILON)
            sense3 = dir1 > 0 ? 1 : -1;
        if(sense3 > 0){
            double sense4 = (pos1 - len1) * obj->cos_sita - pos2 * obj->sin_sita + 0.5 * dis2;
            if(fabs(sense4) < EPSILON){
                double sense_dir = dir1 * obj->cos_sita - dir2 * obj->sin_sita;
                sense4 = sense_dir > 0 ? 1 : -1;
            }
            if(sense4 > 0)
                i1 = i1 + 1;
            else
                i2 = i2 + 1;
        } else {
            double sense5 = pos1 * obj->cos_sita + pos2 * obj->sin_sita - 0.5 * dis2;
            if(fabs(sense5) < EPSILON){
                double sense_dir = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
                sense5 = sense_dir > 0 ? 1 : -1;
            }
            if(sense5 > 0)
                i2 = i2 + 1;
        }
    }

    /////////////////////////////// convert to single-number index //////////////////////////////////
    int nLat_index = -1;
    if(i1 >= 0 && i2 >= 0 && i1 < obj->scope[0] && i2 < obj->scope[1])
        nLat_index = 1 + i1 + obj->scope[0] * i2;

    return nLat_index;
}