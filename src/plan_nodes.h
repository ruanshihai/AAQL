#ifndef AAQL_PLAN_NODES_H
#define AAQL_PLAN_NODES_H


#include <set>
#include "nodes.h"
#include "aql_func.h"

using namespace std;

extern AQLFunc *AQL_FUNC;

class PlanFunc : public NodeFunc {
public:
    enum PLAN_FUNC_RETURN_TYPE {
        VOID, INT, STRING, SPAN, BOOL
    };
    int ret_type;

    PlanFunc(string func_name_="", int ret_type_=VOID)
            : NodeFunc(func_name_), ret_type(ret_type_) {}
    virtual ~PlanFunc() {}

    void *eval();
    void *eval(map<string, Cell> &tuple);
    void *eval(map<string, Cell> &tuple1, map<string, Cell> &tuple2);
    void get_related_views(set<string> &rvs);
    virtual void dump(int spaces=0, int tab=4) override;

    static PlanFunc *make_func(NodeFunc *func);
    static PlanFunc *make_func(NodeFunc *func, map<string, string> alias);
    static PlanFunc *make_follow_tok(string lcol, string rcol, int min, int max);
    static PlanFunc *make_combine_spans(string lcol, string rcol);
};

class PlanArg : public NodeArg {
public:
    string column;

    PlanArg(int type_=ARG_NONE) : NodeArg(type_) {}
    virtual ~PlanArg() {}

    void *eval();
    void *eval(map<string, Cell> &tuple);
    void get_related_views(set<string> &rvs);
    virtual void dump(int spaces=0, int tab=4) override;

    static PlanArg *make_arg(NodeArg *arg);
    static PlanArg *make_arg(NodeArg *arg, map<string, string> alias);
};


#endif //AAQL_PLAN_NODES_H
