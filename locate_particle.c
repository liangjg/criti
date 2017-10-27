//
//  locate_particle.c
//  ttyw
//
//  Created by 叶鑫 on 2017/10/27.
//

#include "geometry.h"
#include "particle_state.h"
#include "universe.h"
#include "cell.h"

extern particle_state_t base_par_state;

extern map *base_univs;

extern map *base_cells;

extern map *base_surfs;

int locate_particle(){
    int univ_index = 0;
    int found_cell = -1;
    double local_pos_temp[3];
    double local_dir_temp[3];
    
    for(int i = 0; i < 3; i++){
        local_pos_temp[i] = base_par_state.pos[i];
        local_dir_temp[i] = base_par_state.dir[i];
    }
    
    int level = 0;
    
    universe_t *univ;
    cell_t *cell;
    
    while(true){
        if(++level > 99){
            puts("terminate locating particle because of too many levels (>99).");
            return -1;
        }
        
        univ = (universe_t *)map_get(base_univs, univ_index);
        
        if(univ->is_lattice){
            
        }
        else{
            
        }
    }
    return found_cell;
}
