#include "plan_nodes.h"

void *PlanFunc::eval() {
    map<string, Cell> tmp;
    return eval(tmp);
}

void *PlanFunc::eval(map<string, Cell> &tuple) {
    // TODO(ruanshihai): low performance due to redundant argument calculation
    if (func_name == "FollowTok") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        int *min = (int *)((PlanArg *)args[2])->eval(tuple);
        int *max = (int *)((PlanArg *)args[3])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->FollowTok(*lhs, *rhs, *min, *max);
        delete(lhs);
        delete(rhs);
        delete(min);
        delete(max);
        return res;

    } else if (func_name == "CombineSpans") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        Span *res = new Span();

        *res = AQL_FUNC->CombineSpans(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;

    } else if (func_name == "overlap") {
        Span *lhs = (Span *)(((PlanArg *)args[0])->eval(tuple));
        Span *rhs = (Span *)(((PlanArg *)args[1])->eval(tuple));
        bool *res = new bool();
        
        *res = AQL_FUNC->overlap(*lhs, *rhs);

        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "Or") {
        bool *lhs = (bool *)((PlanArg *)args[0])->eval(tuple);
        bool *rhs = (bool *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->Or(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "And") {
        bool *lhs = (bool *)((PlanArg *)args[0])->eval(tuple);
        bool *rhs = (bool *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->And(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "Not") {
        bool *hs = (bool *)((PlanArg *)args[0])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->Not(*hs);
        delete(hs);
        return res;
    } else if (func_name == "Longer") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->overlap(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "Shorter") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->overlap(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "Earlier") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->overlap(*lhs, *rhs);

        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "Later") {
        Span *lhs = (Span *)((PlanArg *)args[0])->eval(tuple);
        Span *rhs = (Span *)((PlanArg *)args[1])->eval(tuple);
        bool *res = new bool();

        *res = AQL_FUNC->overlap(*lhs, *rhs);
        delete(lhs);
        delete(rhs);
        return res;
    } else if (func_name == "True") {
        bool *res = new bool();
        *res = AQL_FUNC->True();
        return res;
    }

    return NULL;
}


void PlanFunc::get_related_views(set<string> &rvs) {
    for (auto x : this->args)
        ((PlanArg *)x)->get_related_views(rvs);
}

void PlanFunc::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+FuncName\n", spaces, "");
    fprintf(stdout, "%*s%s\n", spaces+tab, "", func_name.c_str());
    if (!args.empty()) {
        fprintf(stdout, "%*s+Args\n", spaces, "");
        for (auto x : args)
            x->dump(spaces+tab, tab);
    }
}

// TODO(ruanshihai): monitor function additions, deletions and modifications
PlanFunc *PlanFunc::make_func(NodeFunc *func) {
    int ret_type = PlanFunc::VOID;

    if (func->func_name == "FollowTok") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "CombineSpans") {
        ret_type = PlanFunc::SPAN;
    } else if (func->func_name == "overlap") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Or") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "And") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Not") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Longer") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Shorter") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Earlier") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Later") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "True") {
        ret_type = PlanFunc::BOOL;
    }

    PlanFunc *pfunc = new PlanFunc(func->func_name, ret_type);
    for (auto x : func->args)
        pfunc->args.push_back(PlanArg::make_arg((NodeArg *)x));

    return pfunc;
}
/*
PlanFunc *PlanFunc::make_func(NodeFunc *func, map<string, string> alias) {
    int ret_type = PlanFunc::VOID;

    if (func->func_name == "overlap") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Or") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "And") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Not") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Longer") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Shorter") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Earlier") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "Later") {
        ret_type = PlanFunc::BOOL;
    } else if (func->func_name == "True") {
        ret_type = PlanFunc::BOOL;
    }

    PlanFunc *pfunc = new PlanFunc(func->func_name, ret_type);
    for (auto x : func->args)
        pfunc->args.push_back(PlanArg::make_arg((NodeArg *)x, alias));

    return pfunc;
}*/

PlanFunc *PlanFunc::make_follow_tok(string lcol, string rcol, int min, int max) {
    PlanFunc *pfunc = new PlanFunc("FollowTok", PlanFunc::BOOL);

    PlanArg *lhs = new PlanArg(ARG_COLUMN);
    lhs->column = lcol;

    PlanArg *rhs = new PlanArg(ARG_COLUMN);
    rhs->column = rcol;

    PlanArg *low = new PlanArg(ARG_NUM);
    low->n = min;

    PlanArg *high = new PlanArg(ARG_NUM);
    high->n = max;

    pfunc->args.push_back(lhs);
    pfunc->args.push_back(rhs);
    pfunc->args.push_back(low);
    pfunc->args.push_back(high);

    return pfunc;
}

PlanFunc *PlanFunc::make_combine_spans(string lcol, string rcol) {
    PlanFunc *pfunc = new PlanFunc("CombineSpans", PlanFunc::SPAN);

    PlanArg *lhs = new PlanArg(ARG_COLUMN);
    lhs->column = lcol;

    PlanArg *rhs = new PlanArg(ARG_COLUMN);
    rhs->column = rcol;

    pfunc->args.push_back(lhs);
    pfunc->args.push_back(rhs);

    return pfunc;
}

void *PlanArg::eval() {
    map<string, Cell> tmp;
    return eval(tmp);
}

void *PlanArg::eval(map<string, Cell> &tuple) {
    if (this->type == ARG_NUM) {
        int *n = new int(this->n);
        return n;

    } else if (this->type == ARG_STRING) {
        string *s = new string(this->word);
        return s;

    } else if (this->type == ARG_COLUMN) {
        switch (tuple[this->column].type) {
            case Cell::NUM:
                return new int(tuple[this->column].n);
            case Cell::STRING:
                return new string(tuple[this->column].word);
            case Cell::SPAN:
                return new Span(tuple[this->column].span);
            default:
                return NULL;  // should not go here
        }

    } else if (this->type == ARG_FUNC) {
        return ((PlanFunc *)this->func)->eval(tuple);

    } else if (this->type == ARG_STAR) {
        // TODO(ruanshihai): '*' should be supported
    }

    return NULL;
}

void PlanArg::get_related_views(set<string> &rvs) {
    if (this->type == ARG_COLUMN) {
        rvs.insert(this->column.substr(0, this->column.find_first_of(".")));
    } else if (this->type == ARG_FUNC) {
        ((PlanFunc *)this->func)->get_related_views(rvs);
    }
}

void PlanArg::dump(int spaces, int tab) {
    switch (this->type) {
        case ARG_NUM:
            fprintf(stdout, "%*s+NUM\n", spaces, "");
            fprintf(stdout, "%*s%d\n", spaces+tab, "", this->n);
            break;
        case ARG_STRING:
            fprintf(stdout, "%*s+STRING\n", spaces, "");
            fprintf(stdout, "%*s'%s'\n", spaces+tab, "", this->word.c_str());
            break;
        case ARG_COLUMN:
            fprintf(stdout, "%*s+Column\n", spaces, "");
            fprintf(stdout, "%*s'%s'\n", spaces+tab, "", this->column.c_str());
            break;
        case ARG_FUNC:
            this->func->dump(spaces, tab);
            break;
        case ARG_STAR:
            fprintf(stdout, "*\n");
            break;
        default:
            // should not go here
            break;
    }
}

PlanArg *PlanArg::make_arg(NodeArg *arg) {
    PlanArg *parg = new PlanArg();

    parg->type = arg->type;
    if (arg->type == ARG_NUM) {
        parg->n = arg->n;
    } else if (arg->type == ARG_STRING) {
        parg->word = arg->word;
    } else if (arg->type == ARG_COLUMN) {
        parg->column = ((NodeColumn *)arg->column)->view_name
                       +"."
                       +((NodeColumn *)arg->column)->col_name;
    } else if (arg->type == ARG_FUNC) {
        parg->func = PlanFunc::make_func((NodeFunc *)arg->func);
    }

    return parg;
}
/*
PlanArg *PlanArg::make_arg(NodeArg *arg, map<string, string> alias) {
    PlanArg *parg = new PlanArg();

    parg->type = arg->type;
    if (arg->type == ARG_NUM) {
        parg->n = arg->n;
    } else if (arg->type == ARG_STRING) {
        parg->word = arg->word;
    } else if (arg->type == ARG_COLUMN) {
        parg->column = alias.find(((NodeColumn *)arg->column)->view_name)->second
                       +"."
                       +((NodeColumn *)arg->column)->col_name;
    } else if (arg->type == ARG_FUNC) {
        parg->func = PlanFunc::make_func((NodeFunc *)arg->func);
    }

        return parg;
}*/
