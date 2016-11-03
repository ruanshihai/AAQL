#ifndef AAQL_OP_OUTPUT_H
#define AAQL_OP_OUTPUT_H


#include "operator.h"

class OpOutput : public Operator {
public:
    OpOutput(Storage *store_, string view_name_, string alias_="") : Operator(OP_OUTPUT) {
        store = store_;
        view_name = view_name_;
        alias = alias_;
    }
    virtual ~OpOutput() {}

    virtual void execute() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Storage *store;
    string view_name;
    string alias;
};


#endif //AAQL_OP_OUTPUT_H
