#ifndef AAQL_OP_PROJECT_H
#define AAQL_OP_PROJECT_H


#include "operator.h"

enum PROJ_ACTION_TYPE {
    PROJ_ADD_PREFIX,
    PROJ_SELECT_COLS,
    PROJ_RENAME_COLS,
    PROJ_COPY_COLS,
    PROJ_NEW_NUM_COLUMN,
    PROJ_NEW_STRING_COLUMN
};

class OpProject : public Operator {
public:
    // PROJ_ADD_PREFIX
    OpProject(int action_, Operator *child_, string prefix_)
            : Operator(OP_PROJECT, child_->is_optional) {
        action = action_;
        child = child_;
        prefix = prefix_;
    }

    // PROJ_SELECT_COLS
    OpProject(int action_, Operator *child_, vector<string> &cols_)
            : Operator(OP_PROJECT, child_->is_optional) {
        action = action_;
        child = child_;
        cols = cols_;
    }

    // PROJ_RENAME_COLS or PROJ_COPY_COLS
    OpProject(int action_, Operator *child_, map<string, string> &col_pairs_)
            : Operator(OP_PROJECT, child_->is_optional) {
        action = action_;
        child = child_;
        col_pairs = col_pairs_;
    }

    // PROJ_NEW_NUM_COLUMN
    OpProject(int action_, Operator *child_, int n_, string alias_)
            : Operator(OP_PROJECT, child_->is_optional) {
        action = action_;
        child = child_;
        n = n_;
        alias = alias_;
    }

    // PROJ_NEW_STRING_COLUMN
    OpProject(int action_, Operator *child_, string word_, string alias_)
            : Operator(OP_PROJECT, child_->is_optional) {
        action = action_;
        child = child_;
        word = word_;
        alias = alias_;
    }
    virtual ~OpProject() { delete(child); }

    virtual View eval() override;
    virtual void dump(int spaces=0, int tab=4) override;

private:
    Operator *child;
    int action;
    string prefix;
    int n;
    string word;
    string alias;
    vector<string> cols;
    map<string, string> col_pairs;
};


#endif //AAQL_OP_PROJECT_H
