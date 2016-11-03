#ifndef AAQL_OP_SCAN_H
#define AAQL_OP_SCAN_H


#include "operator.h"

class OpScan : public Operator {
public:
    OpScan(Storage *store_, string view_name_) : Operator(OP_SCAN) {
        store = store_;
        view_name = view_name_;
    }
    virtual ~OpScan() {}

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Storage *store;
    string view_name;
};


#endif //AAQL_OP_SCAN_H
