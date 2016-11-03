#ifndef AAQL_OP_CREATE_DICT_H
#define AAQL_OP_CREATE_DICT_H


#include "operator.h"

class OpCreateDict : public Operator {
public:
    OpCreateDict(Storage *store_, string dict_name_, string base_dir_,
                 string dict_file_, vector<string> &words_)
            : Operator(OP_CREATE_DICT) {
        store = store_;
        dict_name = dict_name_;
        base_dir = base_dir_;
        dict_file = dict_file_;
        words = words_;
    }
    virtual ~OpCreateDict() {}

    virtual void execute() override;
    virtual void dump(int spaces=0, int tab=4) override;
    string get_dict_name() { return this->dict_name; }

private:
    Storage *store;
    string dict_name;
    string dict_file;
    string base_dir;
    vector<string> words;
};


#endif //AAQL_OP_CREATE_DICT_H
