#ifndef AAQL_OP_REGEX_H
#define AAQL_OP_REGEX_H


#include <regex>
#include "operator.h"

class OpRegex : public Operator {
public:
    OpRegex(Storage *store_, string reg_expr_) : Operator(OP_REGEX) {
        store = store_;
        reg_expr = reg_expr_;
    }
    virtual ~OpRegex() {}

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Storage *store;
    string reg_expr;
};


#endif //AAQL_OP_REGEX_H
