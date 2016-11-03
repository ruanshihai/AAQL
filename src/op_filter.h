#ifndef AAQL_OP_FILTER_H
#define AAQL_OP_FILTER_H


#include "operator.h"

class OpFilter : public Operator {
public:
    OpFilter(Operator *child_, vector<PlanFunc *> &filters_)
            : Operator(OP_JOIN, child_->is_optional) {
        child = child_;
        filters = filters_;
    }
    virtual ~OpFilter() {
        delete(child);
        for (auto x : filters)
            delete(x);
    }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *child;
    vector<PlanFunc *> filters;
};


#endif //AAQL_OP_FILTER_H
