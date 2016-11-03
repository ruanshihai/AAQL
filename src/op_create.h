#ifndef AAQL_OP_CREATE_H
#define AAQL_OP_CREATE_H


#include "operator.h"

class OpCreate : public Operator {
public:
    OpCreate(Storage *store_, Operator *child_, string view_name_) : Operator(OP_CREATE_VIEW) {
        store = store_;
        child = child_;
        view_name = view_name_;
    }
    virtual ~OpCreate() { delete(child); }

    virtual void execute() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Storage *store;
    Operator *child;
    string view_name;
};


#endif //AAQL_OP_CREATE_H
