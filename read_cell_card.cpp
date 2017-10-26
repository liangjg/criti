//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "map.h"
#include "cell.h"
#include <vector>
#include "stack.h"

extern map *base_cells;
extern IOfp_t base_IOfp;

/* -------------------------- private prototypes ---------------------------- */
char *_generate_rpn(char *);

char _order_between(char a, char b);

int _identify_cell_kw(char *kw);

/* ----------------------------- API implementation ------------------------- */
void read_cell_card(universe_t *univ){
    char buf[256];
    char *ret = nullptr;
    std::vector<int> cells;

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
        cells.push_back(index);

        char *rpn_start = ret;
        while(!ISALPHA(*ret)) ret++;
        *(ret - 1) = 0;
        cell->rpn = _generate_rpn(rpn_start);
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

    if(!cells.empty()){
        univ->contain_cell_num = cells.size();
        univ->fill_cells = new int[univ->contain_cell_num];
        for(int i = 0; i < cells.size(); i++)
            univ->fill_cells[i] = cells[i];
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

char *_generate_rpn(char *exp){
#define TOP(s)    (*(char *)stack_top((s)))
#define POP(s)    (stack_pop((s)))

    size_t len = strlen(exp);
    char *rpn = new char[2 * len];
    size_t i = 0;

    stack *optr = stack_init(sizeof(char));

    char c = '\0';
    stack_push(optr, &c);

    while(optr->_size){
        if(ISNUMBER(*exp)){
            do{
                rpn[i++] = *exp++;
            } while(ISNUMBER(*exp));
            rpn[i++] = ' ';
        }
        else if(*exp == '-'){
            if(*(exp - 1) == '-'){
                printf("ERROR: too many minus(-) before digit\n");
                exit(0);
            }
            rpn[i++] = *exp++;
        }
        else if(ISSPACE(*exp)){
            exp++;
            continue;
        }
        else{
            switch(_order_between(TOP(optr), *exp)){
                case '<':
                    c = *exp++;
                    stack_push(optr, &c);
                    break;
                case '=': /* 只可能是左括号碰到右括号，或者头哨兵碰到尾哨兵 */
                    POP(optr);
                    exp++;
                    break;
                case '>':
                    c = TOP(optr);
                    POP(optr);
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
    stack_free(optr);
    return rpn;
}

char _order_between(char a, char b){
    int p, q;
    switch(a){
        case '&':
            p = INTER;
            break;
        case ':':
            p = UNION;
            break;
        case '!':
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