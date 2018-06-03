//
// Created by x1314aq on 18-5-28.
//

#include "IO_releated.h"
#include "tally.h"


#define ERR_CELL_CARD    1
#define INITIAL_CAP      4

extern IOfp_t base_IOfp;
extern tally_t base_tally;

static cell_tally_t *
_read_cell_tally(int);

static int
_parse_cell(char *,
            cell_tally_t *);

static mesh_tally_t *
_read_mesh_tally(int);

static void
_read_xs_tally();

void
read_tally_block()
{
    char buf[64];
    int id;
    char *c;
    int cell_tally_cap, mesh_tally_cap;
    void *old_addr;

    cell_tally_cap = INITIAL_CAP;
    mesh_tally_cap = INITIAL_CAP;

    base_tally.cell_tallies = calloc(cell_tally_cap, sizeof(cell_tally_t *));
    base_tally.mesh_tallies = calloc(mesh_tally_cap, sizeof(mesh_tally_t *));

    while(fscanf(base_IOfp.inp_fp, "%s %d", buf, &id) == 2) {
        for(c = buf; *c; c++)
            *c = TOUPPER(*c);

        if(strcmp(buf, "CELLTALLY") == 0) {
            base_tally.cell_tallies[base_tally.cell_tally_sz++] = _read_cell_tally(id);
            if(base_tally.cell_tally_sz == cell_tally_cap) {
                old_addr = base_tally.cell_tallies;
                cell_tally_cap <<= 1;
                base_tally.cell_tallies = calloc(cell_tally_cap, sizeof(cell_tally_t *));
                memcpy(base_tally.cell_tallies, old_addr, base_tally.cell_tally_sz * sizeof(cell_tally_t *));
                free(old_addr);
            }
        } else if(strcmp(buf, "MESHTALLY") == 0) {
            base_tally.mesh_tallies[base_tally.mesh_tally_sz++] = _read_mesh_tally(id);
            if(base_tally.mesh_tally_sz == mesh_tally_cap) {
                old_addr = base_tally.mesh_tallies;
                mesh_tally_cap <<= 1;
                base_tally.mesh_tallies = calloc(mesh_tally_cap, sizeof(mesh_tally_t *));
                memcpy(base_tally.mesh_tallies, old_addr, base_tally.mesh_tally_sz * sizeof(mesh_tally_t *));
                free(old_addr);
            }
        } else if(strcmp(buf, "CSTALLY") == 0)
            _read_xs_tally();
        else {
            printf("unknown keyword: %s.\n", buf);
            release_resource();
            exit(0);
        }
    }
}

cell_tally_t *
_read_cell_tally(int id)
{
    char buf[128];
    char c;
    int sz, type;
    cell_tally_t *ct = calloc(1, sizeof(cell_tally_t));
    ct->id = id;

    while(1) {
        sz = 0;
        while(c = fgetc(base_IOfp.inp_fp)) {
            if(ISALPHA(c))
                buf[sz++] = TOUPPER(c);
            else if(c == '=') {
                buf[sz] = '\0';
                break;
            } else if(c == EOF)
                goto END;
        }

        if(strcmp(buf, "TYPE") == 0) {
            if(fscanf(base_IOfp.inp_fp, "%d", &type) != 1 || type < 1 || type > 4) {
                printf("Error in reading CellTally %d: wrong tally type.\n", id);
                free(ct);
                return NULL;
            }
            if(type == 1)
                ct->do_tally = ctally_flux;
            else if(type == 2)
                ct->do_tally = ctally_power;
            else if(type == 3)
                ct->do_tally = ctally_fis_rate;
            else
                ct->do_tally = ctally_abs_rate;
        } else if(strcmp(buf, "ENERGY") == 0) {
            double erg;
            int i;
            long pos = ftell(base_IOfp.inp_fp);

            while(fscanf(base_IOfp.inp_fp, "%lf", &erg) == 1)
                ct->erg_bin_sz++;
            fseek(base_IOfp.inp_fp, pos, SEEK_SET);
            ct->erg_bin = calloc(ct->erg_bin_sz, sizeof(double));
            for(i = 0; i < ct->erg_bin_sz; i++)
                fscanf(base_IOfp.inp_fp, "%lf", &ct->erg_bin[i]);
        } else if(strcmp(buf, "FILTER") == 0) {
            int filter;
            int i;
            long pos = ftell(base_IOfp.inp_fp);

            if(ct->vec_sz == 0) {
                while(fscanf(base_IOfp.inp_fp, "%d", &filter) == 1)
                    ct->vec_sz++;
                fseek(base_IOfp.inp_fp, pos, SEEK_SET);
            }
            for(i = 0; i < ct->vec_sz; i++)
                fscanf(base_IOfp.inp_fp, "%d", &ct->filter[i]);
        } else if(strcmp(buf, "INTEGRAL") == 0) {
            int integral;
            int i;
            long pos = ftell(base_IOfp.inp_fp);

            if(ct->vec_sz == 0) {
                while(fscanf(base_IOfp.inp_fp, "%d", &integral) == 1)
                    ct->vec_sz++;
                fseek(base_IOfp.inp_fp, pos, SEEK_SET);
            }
            for(i = 0; i < ct->vec_sz; i++)
                fscanf(base_IOfp.inp_fp, "%d", &ct->integral[i]);
        } else if(strcmp(buf, "CELL") == 0) {
            sz = 0;
            while(1) {
                c = fgetc(base_IOfp.inp_fp);
                if(!ISNUMBER(c) && !ISSPACE(c) && c != ':' && c != '>' && c != '*') {
                    buf[sz] = '\0';
                    fseek(base_IOfp.inp_fp, -1, SEEK_CUR);
                    break;
                }
                buf[sz++] = c;
            }
            if(_parse_cell(buf, ct)) {
                printf("CellTally %d has wrong CELL card.\n", id);
            }
            goto END;
        }
    }
END:
    return ct;
}

mesh_tally_t *
_read_mesh_tally(int id)
{
    char buf[128];
    char c;
    int sz, type;
    int i, tot;
    mesh_tally_t *mt = calloc(1, sizeof(mesh_tally_t));
    mt->id = id;

    while(1) {
        sz = 0;
        while(c = fgetc(base_IOfp.inp_fp)) {
            if(ISALPHA(c))
                buf[sz++] = TOUPPER(c);
            else if(c == '=') {
                buf[sz] = '\0';
                break;
            } else if(c == EOF)
                goto END;
        }

        if(strcmp(buf, "TYPE") == 0) {
            if(fscanf(base_IOfp.inp_fp, "%d", &type) != 1 || type < 1 || type > 4) {
                printf("Error in reading MeshTally %d: wrong TYPE.\n", id);
                if(mt->erg_bin) free(mt->erg_bin);
                free(mt);
                return NULL;
            }
            if(type == 1)
                mt->do_tally = mtally_flux;
            else if(type == 2)
                mt->do_tally = mtally_power;
            else if(type == 3)
                mt->do_tally = mtally_fis_rate;
            else
                mt->do_tally = mtally_abs_rate;
        } else if(strcmp(buf, "ENERGY") == 0) {
            double erg;
            int i;
            long pos = ftell(base_IOfp.inp_fp);

            while(fscanf(base_IOfp.inp_fp, "%lf", &erg) == 1)
                mt->erg_bin_sz++;
            fseek(base_IOfp.inp_fp, pos, SEEK_SET);
            mt->erg_bin = calloc(mt->erg_bin_sz, sizeof(double));
            for(i = 0; i < mt->erg_bin_sz; i++)
                fscanf(base_IOfp.inp_fp, "%lf", &mt->erg_bin[i]);
        } else if(strcmp(buf, "SCOPE") == 0) {
            int x, y, z;
            if(fscanf(base_IOfp.inp_fp, "%d %d %d", &x, &y, &z) != 3) {
                printf("Error in reading MeshTally %d: wrong SCOEP, needed 3 numbers here.\n", id);
                if(mt->erg_bin) free(mt->erg_bin);
                free(mt);
                return NULL;
            }
            mt->scope[0] = abs(x);
            mt->scope[1] = abs(y);
            mt->scope[2] = abs(z);
        } else if(strcmp(buf, "BOUND") == 0) {
            double x_min, x_max, y_min, y_max, z_min, z_max;
            if(fscanf(base_IOfp.inp_fp, "%lf %lf %lf %lf %lf %lf",
                      &x_min, &x_max, &y_min, &y_max, &z_min, &z_max) != 6) {
                printf("Error in reading MeshTally %d: wrong BOUND, needed 6 numbers here.\n", id);
                if(mt->erg_bin) free(mt->erg_bin);
                free(mt);
                return NULL;
            }
            if((mt->scope[0] != 1 && (x_min < 0.0 || x_max <= x_min))
               || (mt->scope[1] != 1 && (y_min < 0.0 || y_max <= y_min))
               || (mt->scope[2] != 1 && (z_min < 0.0 || z_max <= z_min))) {
                printf("Error in reading MeshTally %d: incorrect boundaries.\n", id);
                if(mt->erg_bin) free(mt->erg_bin);
                free(mt);
                return NULL;
            }
            mt->bound_min[0] = x_min;
            mt->bound_min[1] = y_min;
            mt->bound_min[2] = z_min;
            mt->bound_max[0] = x_max;
            mt->bound_max[1] = y_max;
            mt->bound_max[2] = z_max;

            for(i = 0; i < 3; i++)
                mt->delta[i] = (mt->bound_max[i] - mt->bound_min[i]) / mt->scope[i];
        }
    }

END:
    tot = mt->scope[0] * mt->scope[1] * mt->scope[2];
    mt->result = malloc(tot * sizeof(double *));
    for(i = 0; i < tot; i++)
        mt->result[i] = calloc(mt->erg_bin_sz + 1, sizeof(double));    /* 初始化为0 */

    return mt;
}

void
_read_xs_tally()
{

}

/* *********************************************
 * 不支持在一个CellTally中用分离的方式写多个cell， 即
 * cell = 2 3 4 1，这种是非法的；支持使用':'和'*'两种
 * 通配符，'*'通配符只支持"cell = *32"这一种写法
 * *********************************************/
int
_parse_cell(char *buf,
            cell_tally_t *ct)
{
    size_t sz = strlen(buf);
    int num_of_gt = 0;
    int num_of_colon = 0;
    int i;

    for(i = 0; i < sz; i++) {
        if(buf[i] == '>')
            num_of_gt++;
        else if(buf[i] == ':')
            num_of_colon++;
    }

    if(ct->vec_sz == 0)
        ct->vec_sz = num_of_gt + 1;
    else if(ct->vec_sz != num_of_gt + 1)
        return ERR_CELL_CARD;

    int cells_sz = 1;
    int start, stop;
    char *end;
    while(num_of_colon) {
        for(i = sz; i >= 0; i--) {
            if(buf[i] == ':') {
                stop = strtol(&buf[i + 1], &end, 10);
                while(!ISNUMBER(buf[i]))
                    i--;
                while(ISNUMBER(buf[i]))
                    i--;
                start = strtol(&buf[i], &end, 10);
                cells_sz *= stop - start + 1;
                num_of_colon--;
            }
        }
    }
    ct->cells_sz = cells_sz;
    ct->cells = malloc(cells_sz * sizeof(int *));
    for(i = 0; i < cells_sz; i++)
        ct->cells[i] = malloc(ct->vec_sz * sizeof(int));

    return 0;
}
