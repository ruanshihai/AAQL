#ifndef AAQL_OP_JOIN_H
#define AAQL_OP_JOIN_H


#include "operator.h"

class OpJoin : public Operator {
public:
    OpJoin(Operator *left_, Operator *right_, vector<PlanFunc *> &conds_)
            : Operator(OP_JOIN) {
        left = left_;
        right = right_;
        conds = conds_;
    }
    virtual ~OpJoin() {
        delete(left);
        delete(right);
        for (auto x : conds)
            delete(x);
    }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *left;
    Operator *right;
    vector<PlanFunc *> conds;
};


#endif //AAQL_OP_JOIN_H
