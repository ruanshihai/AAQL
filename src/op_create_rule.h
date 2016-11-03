#ifndef AAQL_OP_CREATE_RULE_H
#define AAQL_OP_CREATE_RULE_H

#include "operator.h"

class OpCreateRule : public Operator {
public:
    OpCreateRule(Storage *store_, PlanFunc *apply_, PlanFunc *when_, string row1_, string row2_, map<string, string> rows_, string rule_name_) : Operator(OP_CREATE_RULE) {
        store = store_;
        apply = apply_;
        when = when_;
        row1 = row1_;
        row2 = row2_;
        rows = rows_;
        rule_name = rule_name_;
    }
    virtual ~OpCreateRule() { delete(store); }

    virtual void execute() override;
    virtual void dump(int spaces=0, int tab=4) override;

    string rule_name;
private:
    Storage *store;
    PlanFunc *apply;
    PlanFunc *when;
    string row1;
    string row2;
    map<string, string> rows;
};


#endif //AAQL_OP_CREATE_RULE_H
