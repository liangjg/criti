//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "cell.h"
#include "map.h"
#include <stack>
#include <string>
#include <vector>


extern map *base_cells;
extern IOfp_t base_IOfp;

static int cell_index;

/* -------------------------- private prototypes ---------------------------- */
char *
_generate_rpn(const char *exp,
              bool &is_simple);

char
_order_between(char a,
               char b,
               bool &is_simple);

int
_identify_cell_kw(char *kw);

void
_extract_surfs_from_rpn(cell_t *cell);

void
_simplify(const char *exp,
          char **simplified_exp);

void
_transform(std::string &s);

/* ----------------------------- API implementation ------------------------- */
void
read_cell_card(universe_t *univ)
{
    int i;
    std::vector<void *> cells;    /* 存储每个cell实例的地址 */
    char buf[256];
    int index;
    char *rpn_start;
    char *ret = NULL;
    bool is_simple = true;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        while(ISSPACE(*ret)) ret++;
        if(ISCOMMENT(*ret)) continue;  /* 当前行是以注释符开头的，继续读取下一行 */
        if(ISRETURN(*ret)) break;      /* 当前行是空行，意味着当前UNIVERSE已经结束了 */

        while(!ISNUMBER(*ret)) ret++;
        index = 0;
        do {
            index *= 10;
            index += *ret++ - '0';
        } while(ISNUMBER(*ret));

        cell_t *cell = cell_init();
        cell->id = index;
        cell->parent = univ;
        cell_index = index;
        map_put(base_cells, index, cell);
        cells.push_back(cell);

        rpn_start = ret;
        while(!ISALPHA(*ret)) {
            if(*ret != '+' && *ret != '-' &&
               !ISNUMBER(*ret) && !ISSPACE(*ret) &&
               *ret != '(' && *ret != ')' &&
               *ret != '&' && *ret != ':' && *ret != '!') {
                printf("cell %d has unexpected character %c.\n", cell_index, *ret);
                release_resource();
                exit(0);
            }
            ret++;
        }
        *(ret - 1) = 0;
        cell->expr = (char *) malloc(sizeof(char) * (ret - rpn_start));
        memcpy(cell->expr, rpn_start, (ret - rpn_start) * sizeof(char));
        cell->rpn = _generate_rpn(rpn_start, is_simple);
        cell->simple = is_simple;
        _extract_surfs_from_rpn(cell);
        while(!ISRETURN(*ret) && !ISCOMMENT(*ret)) {
            char *kw_start = ret;
            while(ISALPHA(*ret)) {
                *ret = TOUPPER(*ret);
                ret++;
            }
            *ret = 0;

            char *end;
            switch(_identify_cell_kw(kw_start)) {
                case 0:    /* fill argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->fill = (void *) strtol(ret, &end, 10);    /* 在preprocess_geometry()中转换为实际的地址 */
                    ret = end;
                    break;
                case 1:    /* mat argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->mat = (void *) strtol(ret, &end, 10);     /* 在preprocess_geometry()中转换为实际的地址 */
                    ret = end;
                    break;
                case 2:    /* imp argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->imp = (int) strtol(ret, &end, 10);
                    ret = end;
                    break;
                case 3:    /* tmp argument */
                    while(!ISNUMBER(*ret)) ret++;
                    cell->tmp = strtod(ret, &end) * 1.3806505E-23 / (1.6022E-19) * 1.0E-6;
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
                default:break;
            }
            while(ISSPACE(*ret)) ret++;
        }
    }
    univ->cells_sz = cells.size();
    univ->cells = (void **) malloc(univ->cells_sz * sizeof(void *));
    for(i = 0; i < univ->cells_sz; i++)
        univ->cells[i] = cells[i];
}

/* ------------------------ private API implementation ---------------------- */
int
_identify_cell_kw(char *kw)
{
    int i;
    for(i = 0; i < CELL_KW_NUMBER; i++) {
        if(strcmp(kw, cell_kw[i]) == 0)
            return i;
    }
    return -1;
}

char *
_generate_rpn(const char *exp,
              bool &is_simple)
{
    size_t len;
    char *rpn;
    char *simplified_exp;
    /* *************************************************************
     * 将原表达式中的'!'运算符去掉，一定程度上简化表达式；
     * 其中simplified_exp指向由_simplify分配好的地址，存放化简后的表达式；
     * 在此函数结束处释放掉
     * *************************************************************/
    _simplify(exp, &simplified_exp);
    exp = simplified_exp;

    len = strlen(exp);
    rpn = (char *) malloc(2 * len * sizeof(char));
    size_t i = 0;
    char c = '\0';

    std::stack<char> optr;
    optr.push(c);

    while(!optr.empty()) {
        if(ISNUMBER(*exp)) {
            do {
                rpn[i++] = *exp++;
            } while(ISNUMBER(*exp));
            rpn[i++] = ' ';
        } else if(*exp == '-') {
            rpn[i++] = *exp++;
        } else if(ISSPACE(*exp)) {
            exp++;
            continue;
        } else {
            switch(_order_between(optr.top(), *exp, is_simple)) {
                case '<':c = *exp++;
                    optr.push(c);
                    break;
                case '=': /* 只可能是左括号碰到右括号，或者头哨兵碰到尾哨兵 */
                    optr.pop();
                    exp++;
                    break;
                case '>':c = optr.top();
                    optr.pop();
                    rpn[i++] = c;
                    rpn[i++] = ' ';
                    break;
                default:printf("Error expression!\n");
                    release_resource();
                    exit(0);
            }
        }
    }
    rpn[i] = '\0';
    free(simplified_exp);
    return rpn;
}

char
_order_between(char a,
               char b,
               bool &is_simple)
{
    int p, q;
    switch(a) {
        case '&':p = INTER;
            break;
        case ':':is_simple = false;
            p = UNION;
            break;
        case '!':is_simple = false;
            p = COMPLEMENT;
            break;
        case '(':p = L_P;
            break;
        case ')':p = R_P;
            break;
        case '\0':p = EOE;
            break;
        default:printf("Error: unknown operator %c\n", a);
            release_resource();
            exit(0);
    }
    switch(b) {
        case '&':q = INTER;
            break;
        case ':':q = UNION;
            break;
        case '!':q = COMPLEMENT;
            break;
        case '(':q = L_P;
            break;
        case ')':q = R_P;
            break;
        case '\0':q = EOE;
            break;
        default:printf("Error: unknown operator %c\n", b);
            release_resource();
            exit(0);
    }
    return priority[p][q];
}

void
_extract_surfs_from_rpn(cell_t *cell)
{
    int i;
    std::vector<int> surfs;
    int surf_index = 0;
    char *start = cell->rpn;

    while(*start != '\0') {
        if(ISNUMBER(*start)) {
            surf_index = 0;
            do {
                surf_index *= 10;
                surf_index += *start++ - '0';
            } while(ISNUMBER(*start));
            surfs.push_back(surf_index);
        } else if(*start == '-') {
            surf_index = 0;
            start++;
            do {
                surf_index *= 10;
                surf_index += *start++ - '0';
            } while(ISNUMBER(*start));
            surf_index = (~surf_index) + 1;
            surfs.push_back(surf_index);
        }
        start++;
    }
    cell->surfs_sz = surfs.size();
    cell->surfs = (int *) malloc(cell->surfs_sz * sizeof(int));
    for(i = 0; i < cell->surfs_sz; i++)
        cell->surfs[i] = surfs[i];
}

void
_simplify(const char *exp,
          char **simplified_exp)
{
    std::string s(exp);

    size_t found = 0;
    size_t start, pos;
    int num_of_lp = 0;

    while((found = s.find('!')) != std::string::npos) {
        s.erase(found, 1);
        start = found;
        while(s[start] != '(' && !ISNUMBER(s[start])) start++;
        if(s[start] == '(') {    /* 面布尔表达式 */
            num_of_lp++;
            pos = start + 1;
            while(num_of_lp && s[pos]) {
                if(s[pos] == '(') num_of_lp++;
                else if(s[pos] == ')') num_of_lp--;
                pos++;
            }
            if(num_of_lp) {
                printf("cell %d has mismatched parentheses: %s.\n", cell_index, exp);
                release_resource();
                exit(0);
            }
            std::string sub_s = s.substr(start, pos - start);
            _transform(sub_s);
            s.replace(start, pos - start, sub_s);
        } else {    /* 此时表示对栅元取非 */
            int cell_id = s[start] - '0';
            pos = start + 1;
            while(ISNUMBER(s[pos])){
                cell_id *= 10;
                cell_id += s[pos++] - '0';
            }
            cell_t *cell = (cell_t *) map_get(base_cells, cell_id);
            if(!cell) {
                printf("cell %d has wrong expression: specified cell %d has not been initialized.\n", cell_index, cell_id);
                release_resource();
                exit(0);
            }
            std::string cell_expr(cell->expr);
            cell_expr.insert(0, 1, '(');
            cell_expr.insert(0, 1, '!');
            cell_expr.push_back(')');
            s.replace(start, pos, cell_expr);
            continue;
        }
    }

    size_t sz = s.size() + 1;
    *simplified_exp = (char *) malloc(sz * sizeof(char));
    strcpy(*simplified_exp, s.c_str());
}

void
_transform(std::string &s)
{
    size_t len = s.size();
    size_t i = 0;
    while(i < len) {
        if(s[i] == '&') s[i++] = ':';
        else if(s[i] == ':') s[i++] = '&';
        else if(s[i] == '-') {
            s[i++] = ' ';
            while(ISNUMBER(s[i])) i++;
        } else if(ISNUMBER(s[i])) {
            s.insert(i, 1, '-');
            len++;
            i += 2;
            while(ISNUMBER(s[i])) i++;
        } else i++;
    }
}