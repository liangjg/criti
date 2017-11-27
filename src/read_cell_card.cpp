//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "cell.h"
#include <stack>

extern map *base_cells;
extern IOfp_t base_IOfp;

/* -------------------------- private prototypes ---------------------------- */
char *_generate_rpn(char *exp, bool &is_simple);

char _order_between(char a, char b, bool &is_simple);

int _identify_cell_kw(char *kw);

void _extract_surfs_from_rpn(cell_t *cell);

/* ----------------------------- API implementation ------------------------- */
void read_cell_card(universe_t *univ){
    char buf[256];
    char *ret = nullptr;
    bool is_simple = true;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp)) != nullptr){
        while(ISSPACE(*ret)) ret++;
        if(ISRETURN(*ret)) break;    /* current line is blank, current UNIVERSE block terminates */

        while(!ISNUMBER(*ret)) ret++;
        int index = 0;
        do{
            index *= 10;
            index += *ret++ - '0';
        } while(ISNUMBER(*ret));

        cell_t *cell = cell_init();
        map_put(base_cells, index, cell);
        vector_push_back(&univ->fill_cells, &index);

        char *rpn_start = ret;
        while(!ISALPHA(*ret)) ret++;
        *(ret - 1) = 0;
        cell->rpn = _generate_rpn(rpn_start, is_simple);
        cell->simple = is_simple;
        _extract_surfs_from_rpn(cell);
        while(!ISRETURN(*ret) && !ISCOMMENT(*ret)){
            char *kw_start = ret;
            while(ISALPHA(*ret)){
                *ret = TOUPPER(*ret);
                ret++;
            }
            *ret = 0;

            char *end;
            switch(_identify_cell_kw(kw_start)){
                case 0:    /* fill argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->fill = strtol(ret, &end, 10);
                    ret = end;
                    break;
                case 1:    /* mat argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->mat = strtol(ret, &end, 10);
                    ret = end;
                    break;
                case 2:    /* imp argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->imp = strtol(ret, &end, 10);
                    ret = end;
                    break;
                case 3:    /* tmp argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->tmp = strtod(ret, &end);
                    ret = end;
                    break;
                case 4:    /* vol argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->vol = strtod(ret, &end);
                    ret = end;
                    break;
                case 5:    /* inner argument */
                    while(!ISNUMBER(*ret)) ret++;
                    if(strtol(ret, &end, 10) == 1)
                        cell->is_inner_cell = true;
                    ret = end;
                    break;
                case 6:    /* void argument */
                    while(!ISNUMBER(*ret)) ret++;
                    if(strtol(ret, &end, 10) == 1)
                        cell->imp = 0;
                    ret = end;
                    break;
                default:
                    break;
            }
            while(ISSPACE(*ret)) ret++;
        }
    }
}

/* ------------------------ private API implementation ---------------------- */
int _identify_cell_kw(char *kw){
    int i;
    for(i = 0; i < CELL_KW_NUMBER; i++){
        if(strcmp(kw, cell_kw[i]) == 0)
            return i;
    }
    return -1;
}

char *_generate_rpn(char *exp, bool &is_simple){
    size_t len = strlen(exp);
    char *rpn = new char[2 * len];
    size_t i = 0;
    char c = '\0';

    std::stack<char> optr;
    optr.push(c);

    while(!optr.empty()){
        if(ISNUMBER(*exp)){
            do{
                rpn[i++] = *exp++;
            } while(ISNUMBER(*exp));
            rpn[i++] = ' ';
        }
        else if(*exp == '-'){
            rpn[i++] = *exp++;
        }
        else if(ISSPACE(*exp)){
            exp++;
            continue;
        }
        else{
            switch(_order_between(optr.top(), *exp, is_simple)){
                case '<':
                    c = *exp++;
                    optr.push(c);
                    break;
                case '=': /* 只可能是左括号碰到右括号，或者头哨兵碰到尾哨兵 */
                    optr.pop();
                    exp++;
                    break;
                case '>':
                    c = optr.top();
                    optr.pop();
                    rpn[i++] = c;
                    rpn[i++] = ' ';
                    break;
                default:
                    printf("Error expression!\n");
                    exit(0);
            }
        }
    }
    rpn[i] = '\0';
    return rpn;
}

char _order_between(char a, char b, bool &is_simple){
    int p, q;
    switch(a){
        case '&':
            p = INTER;
            break;
        case ':':
            is_simple = false;
            p = UNION;
            break;
        case '!':
            is_simple = false;
            p = COMPLEMENT;
            break;
        case '(':
            p = L_P;
            break;
        case ')':
            p = R_P;
            break;
        case '\0':
            p = EOE;
            break;
        default:
            printf("Error: unknown operator %c\n", a);
            exit(0);
    }
    switch(b){
        case '&':
            q = INTER;
            break;
        case ':':
            q = UNION;
            break;
        case '!':
            q = COMPLEMENT;
            break;
        case '(':
            q = L_P;
            break;
        case ')':
            q = R_P;
            break;
        case '\0':
            q = EOE;
            break;
        default:
            printf("Error: unknown operator %c\n", b);
            exit(0);
    }
    return priority[p][q];
}

void _extract_surfs_from_rpn(cell_t *cell){
    int surf_index = 0x00000000;
    char *start = cell->rpn;

    while(*start != '\0'){
        if(ISNUMBER(*start)){
            do{
                surf_index *= 10;
                surf_index += *start++ - '0';
            } while(ISNUMBER(*start));
            vector_push_back(&cell->surfs, &surf_index);
            surf_index = 0x00000000;
        }
        else if(*start == '-')
            surf_index |= 0x80000000;
        start++;
    }
}