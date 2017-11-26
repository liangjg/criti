//
// Created by x1314aq on 17-11-26.
//

#include "nuclide.h"

void nuc_free(nuclide_t *obj){
    free(obj->XSS);
    free(obj->MTR_index);
    free(obj->LSIG);
    free(obj->LAND);
    free(obj->LDLW);
    free(obj->fis_XSS);
    free(obj->inel_XSS);
    free(obj);
}