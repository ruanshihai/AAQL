#ifndef AAQL_OP_CLEAN_H
#define AAQL_OP_CLEAN_H


#include "operator.h"

class OpClean : public Operator {
public:
    OpClean(vector<string> clean_list_, vector<Operator *> cleans_, map<string, int> rules_) : Operator(OP_CLEAN) {
        clean_lists = clean_list_;
        cleans = cleans_;
        rules = rules_;
    }
    virtual ~OpClean() {}

    virtual void execute() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    vector<string> clean_lists;
    vector<Operator *> cleans;
    map<string, int> rules;
};


#endif //AAQL_OP_CLEAN_H
