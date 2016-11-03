#ifndef AAQL_OP_DICTIONARY_H
#define AAQL_OP_DICTIONARY_H


#include "operator.h"

class OpDictionary : public Operator {
public:
    OpDictionary(Storage *store_, string dict_name_, string alias_)
            : Operator(OP_DICTIONARY) {
        store = store_;
        dict_name = dict_name_;
        alias = alias_;
    }
    virtual ~OpDictionary() {}

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Storage *store;
    string dict_name;
    string alias;
};


#endif //AAQL_OP_DICTIONARY_H
