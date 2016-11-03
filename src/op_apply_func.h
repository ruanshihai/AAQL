#ifndef AAQL_OP_APPLY_FUNC_H
#define AAQL_OP_APPLY_FUNC_H


#include "operator.h"

enum FUNC_NAME {
    CONTAIN,
    ENCLOSE,
    PRECEDE,
    FORWARDOVERLAP,
    BACKOVERLAP,
    FULLOVERLAP,
    OVERLAP
};

class OpApplyFunc : public Operator {
public:
    OpApplyFunc(Operator *child_, PlanFunc *func_, string new_col_name_)
            : Operator(OP_APPLY_FUNC, child_->is_optional) {
        child = child_;
        func = func_;
        new_col_name = new_col_name_;
    }

    virtual ~OpApplyFunc() {
        delete(child);
        delete(func);
    }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *child;
    PlanFunc *func;
    string new_col_name;
};


#endif //AAQL_OP_APPLY_FUNC_H
