//
// Created by x1314aq on 18-5-28.
//

#ifndef CRITI_TALLY_H
#define CRITI_TALLY_H

#include "cell_tally.h"
#include "mesh_tally.h"

#define HAS_CELL_TALLY(tally)    ((tally)->types & 0x1U)
#define HAS_MESH_TALLY(tally)    ((tally)->types & 0x2U)
#define HAS_XS_TALLY(tally)      ((tally)->types & 0x4U)

#define SET_CELL_TALLY(tally)    ((tally)->types | 0x1U)
#define SET_MESH_TALLY(tally)    ((tally)->types | 0x2U)
#define SET_XS_TALLY(tally)      ((tally)->types | 0x4U)

struct tally_t {
    unsigned int types;
    int cell_tally_sz;
    int mesh_tally_sz;
//    int xs_tally_sz;
    cell_tally_t **cell_tallies;
    mesh_tally_t **mesh_tallies;
//    xs_tally_t **xs_tallies;
};

typedef struct tally_t tally_t;

void tally_free(tally_t *tally);

#endif //CRITI_TALLY_H
