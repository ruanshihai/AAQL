#ifndef AQL_OPERATOR_H
#define AQL_OPERATOR_H


#include "nodes.h"
#include "storage.h"
#include "plan_nodes.h"

enum OP_TYPE {
    OP_NONE,
    OP_DICTIONARY,
    OP_REGEX,
    OP_CREATE_VIEW,
    OP_CREATE_DICT,
    OP_OUTPUT,
    OP_SCAN,
    OP_JOIN,
    OP_PROJECT,
    OP_UNION,
    OP_APPLY_FUNC,
    OP_BLOCK,
    OP_SORT,
    OP_CREATE_RULE,
    OP_CLEAN,

    OP_MINUS,
    OP_GROUP_BY,
    OP_CONSOLIDATE
};

class Operator {
public:
    int type;
    bool is_optional;  // flag for pattern optional
    string leading_col_name;  // guider for join operators in pattern

    Operator(int type_=OP_NONE, bool is_optional_=false) {
        type = type_; is_optional = is_optional_; leading_col_name = "";
    }
    virtual ~Operator() {}

    // entry point for OP_DICTIONARY, OP_REGEX, OP_CREATE_VIEW, OP_CREATE_DICT, OP_OUTPUT
    virtual void execute() {}
    // entry point for the remaining operators in query plan tree
    virtual View eval() { return View(); }
    virtual void dump(int spaces=0, int tab=4) {}

    // help functions to generate query plan
    static string get_tmp_col_name();
    static string get_group_name(int n);
};


#endif //AQL_OPERATOR_H
