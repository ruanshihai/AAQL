#ifndef AAQL_OP_BLOCK_H
#define AAQL_OP_BLOCK_H


#include "operator.h"

class OpBlock : public Operator {
public:
    OpBlock(Operator *child_, int min_, int max_, string new_col_name_)
            : Operator(OP_BLOCK) {
        child = child_;
        min = min_;
        max = max_;
        new_col_name = new_col_name_;
    }
    virtual ~OpBlock() { delete(child); }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *child;
    int min;
    int max;
    string new_col_name;
};


#endif //AAQL_OP_BLOCK_H
