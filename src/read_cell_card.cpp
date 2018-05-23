//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "cell.h"
#include "map.h"
#include <string>
#include <vector>
#include <stack>


extern map *base_cells;
extern IOfp_t base_IOfp;

static int cell_index;

/* -------------------------- private prototypes ---------------------------- */
static char *
_generate_rpn(const char *exp,
              bool &is_simple);

static int
_identify_cell_kw(char *kw);

static void
_extract_surfs_from_rpn(cell_t *cell);

static void
_simplify(const char *exp,
          char **simplified_exp);

static void
_transform(std::string &s);

struct node {
    union {
        char optr;
        int opnd;
    } val;
    struct node *lchild;
    struct node *rchild;
};

static int
_find_lowest_prio_optr(char *);

static bool
_match_pare(const char *,
            int,
            int);

static struct node *
_build_binary_tree(char *);

static struct node *
_do_build(char *);

static void
_post_traversal(struct node *,
                std::string &);

static void
_in_traversal(struct node *,
              std::string &);

static void
_pre_traversal(struct node *,
               std::string &);

static void
_destroy_binary_tree(struct node *);

/* ----------------------------- API implementation ------------------------- */
void
read_cell_card(universe_t *univ)
{
    std::vector<void *> cells;    /* 存储每个cell实例的地址 */
    char buf[256];
    char *ret = NULL;
    bool is_simple = true;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        while(ISSPACE(*ret)) ret++;
        if(ISCOMMENT(*ret)) continue;  /* 当前行是以注释符开头的，继续读取下一行 */
        if(ISRETURN(*ret)) break;      /* 当前行是空行，意味着当前UNIVERSE已经结束了 */

        while(!ISNUMBER(*ret)) ret++;
        int index = 0;
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

        char *rpn_start = ret;
        while(!ISALPHA(*ret)) ret++;
        *(ret - 1) = 0;
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
                cell->tmp = strtod(ret, &end) * 1.3806505E-23/(1.6022E-19)*1.0E-6;    /* convert to KT(MeV) */
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
    for(int i = 0; i < univ->cells_sz; i++)
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
    char *simplified_exp;
    char *rpn;
    struct node *root;
    std::string str;
    std::string infix;

    /* ********************************************
     * 将原表达式中的'!'运算符去掉，一定程度上简化表达式；
     * 其中simplified_exp指向由_simplify分配好的地址，
     * 存放化简后的表达式；在此函数结束处释放掉
     * ********************************************/
    _simplify(exp, &simplified_exp);
    for(char *c = simplified_exp; *c != '\0'; c++)
        if(*c == ':') {
            is_simple = false;
            break;
        }

    root = _build_binary_tree(simplified_exp);
    _post_traversal(root, str);

    rpn = (char *) malloc((str.size() + 1) * sizeof(char));
    memcpy(rpn, str.c_str(), str.size());
    rpn[str.size()] = '\0';
    _in_traversal(root, infix);

    free(simplified_exp);
    _destroy_binary_tree(root);
    return rpn;
}

void
_extract_surfs_from_rpn(cell_t *cell)
{
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
    for(int i = 0; i < cell->surfs_sz; i++)
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
        if(s[start] == '(') {
            num_of_lp++;
            pos = start + 1;
            while(num_of_lp) {
                if(s[pos] == '(') num_of_lp++;
                else if(s[pos] == ')') num_of_lp--;
                pos++;
            }
        }
        std::string sub_s = s.substr(start, pos - start);
        _transform(sub_s);
        s.replace(start, pos - start, sub_s);
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

struct node *
_build_binary_tree(char *exp)
{
    if(!exp) return NULL;
    return _do_build(exp);
}

int
_find_lowest_prio_optr(char *exp)
{
    int num_of_rp = 0;
    int end = strlen(exp);

    for(int i = end - 1; i > 0; i--) {
        if(exp[i] == ')') num_of_rp++;
        else if(exp[i] == '(') num_of_rp--;
        else if(num_of_rp == 0 && (exp[i] == '&' || exp[i] == ':')) return i;
    }
    return -1;
}

bool
_match_pare(const char *exp,
            int start,
            int end)
{
    int num = 0;
    for(int i = start + 1; i < end - 1; i++) {
        if(exp[i] == '(') num++;
        else if(exp[i] == ')') num--;
        if(num < 0) return false;
    }
    return num == 0;
}

struct node *
_do_build(char *exp)
{
    struct node *bin_tree = NULL;
    int start = 0;
    int end;
    int num_of_optr = 0;

    while(!(*exp == '-' || ISNUMBER(*exp) || *exp == '(')) {
        if(*exp != ' ' && *exp != '+') {
            printf("cell %d has wrong expression: %s\n", cell_index, exp);
            exit(1);
        }
        exp++;
    }
    end = strlen(exp);
    while(!(exp[end - 1] == ')' || ISNUMBER(exp[end - 1]))) {
        if(exp[end - 1] != ' ') {
            printf("cell %d has wrong expression: %s\n", cell_index, exp);
            exit(1);
        }
        exp[--end] = '\0';
    }
    while(*exp == '(' && exp[end - 1] == ')' && _match_pare(exp, 0, end)) {
        exp[--end] = '\0';
        *exp++ = '\0';
    }

    end = strlen(exp);
    for(int i = start; i < end; i++) {
        if(exp[i] == '&' || exp[i] == ':')
            num_of_optr++;
    }

    /* ************************************************************************
     * End of recursion.
     * The expectation is that there is only one operand, positive or negative.
     * But in fact there may be three cases.
     * 1. Only one operand left;
     * 2. More than one operand without operators;
     * 3. Nothing at all, no operands between operators;
     * ************************************************************************/
    if(num_of_optr == 0) {
        int digit_start;
        int val = 0;
        if(start == end) {
            printf("cell %d has wrong expression: %s\n", cell_index, exp);
            exit(1);
        }
        digit_start = start;
        if(exp[start] == '-') digit_start++;
        for(int i = digit_start; i < end; i++) {
            if(ISNUMBER(exp[i])) {
                val *= 10;
                val += exp[i] - '0';
            } else    /* 多个运算数之间没有运算符 */
            {
                printf("cell %d has wrong expression: %s\n", cell_index, exp);
                exit(1);
            }
        }
        if(exp[start] == '-') val *= -1;
        bin_tree = (struct node *) malloc(sizeof(struct node));
        bin_tree->val.opnd = val;
        bin_tree->lchild = NULL;
        bin_tree->rchild = NULL;
    } else {
        int med = _find_lowest_prio_optr(exp);
        if(med == -1) {
            printf("cell %d has wrong expression: %s\n", cell_index, exp);
            exit(1);
        }
        bin_tree = (struct node *) malloc(sizeof(struct node));
        bin_tree->val.optr = exp[med];
        exp[med] = '\0';
        bin_tree->lchild = _do_build(exp);
        bin_tree->rchild = _do_build(exp + med + 1);
    }
    return bin_tree;
}

/* **********************************************************
 * Generate RPN through post-order traversal.
 * Of cause, it will generate prefix expression through
 * pre-order traversal, and infix expression through in-order
 * traversal;
 * **********************************************************/
void
_post_traversal(struct node *root,
                std::string &str)
{
    if(!root) return;
    if(root->lchild) _post_traversal(root->lchild, str);
    if(root->rchild) _post_traversal(root->rchild, str);

    if(!root->lchild && !root->rchild)
        str += std::to_string(root->val.opnd) += ' ';
    else {
        str.push_back(root->val.optr);
        str += ' ';
    }
}

void
_in_traversal(struct node *root,
              std::string &str)
{
    if(!root) return;
    if(root->lchild) {
        str.push_back('(');
        _in_traversal(root->lchild, str);
        str.push_back(')');
    }
    if(!root->lchild && !root->rchild)
        str += std::to_string(root->val.opnd);
    else{
        str.push_back(root->val.optr);
        str.push_back('(');
        _in_traversal(root->rchild, str);
        str.push_back(')');
    }
}

void
_destroy_binary_tree(node *root)
{
    if(!root) return;
    if(root->lchild) _destroy_binary_tree(root->lchild);
    if(root->rchild) _destroy_binary_tree(root->rchild);
    free(root);
}
