#ifndef AAQL_OP_UNION_H
#define AAQL_OP_UNION_H


#include "operator.h"

class OpUnion : public Operator {
public:
    OpUnion(Operator *left_, Operator *right_) : Operator(OP_UNION) {
        left = left_;
        right = right_;
    }
    virtual ~OpUnion() {
        delete(left);
        delete(right);
    }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *left;
    Operator *right;
};


#endif //AAQL_OP_UNION_H
