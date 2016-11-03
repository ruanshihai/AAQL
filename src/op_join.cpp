#include "op_join.h"

View OpJoin::eval() {
    View left_vi = left->eval();
    View right_vi = right->eval();
    View res_vi;

    // join's result view consists of all columns both from left view and right view
    for (auto x : left_vi.cols) {
        switch (x.second->type) {
            case COLUMN_NUM:
                res_vi.cols[x.first] = new NumColumn(x.first);
                break;
            case COLUMN_STRING:
                res_vi.cols[x.first] = new StringColumn(x.first);
                break;
            case COLUMN_SPAN:
                res_vi.cols[x.first] = new SpanColumn(x.first);
                break;
            default:
                // should not go here
                break;
        }
    }
    for (auto x : right_vi.cols) {
        switch (x.second->type) {
            case COLUMN_NUM:
                res_vi.cols[x.first] = new NumColumn(x.first);
                break;
            case COLUMN_STRING:
                res_vi.cols[x.first] = new StringColumn(x.first);
                break;
            case COLUMN_SPAN:
                res_vi.cols[x.first] = new SpanColumn(x.first);
                break;
            default:
                // should not go here
                break;
        }
    }

    map<string, Cell> tuple;
    int left_nrows = left_vi.get_nrows();
    int right_nrows = right_vi.get_nrows();

    // nested loop join action
    for (int i=0; i<left_nrows; i++) {
        for (auto x : left_vi.cols)
            tuple[x.first] = left_vi.get_cell(x.first, i);

        for (int j=0; j<right_nrows; j++) {
            for (auto x : right_vi.cols)
                tuple[x.first] = right_vi.get_cell(x.first, j);

            int k;
            for (k=0; k<conds.size(); k++) {
                bool *res = (bool *)conds[k]->eval(tuple);
                if (*res) {
                    delete(res);
                } else {
                    delete(res);
                    break;
                }
            }

            // no conditions or all conditions evaluation are passed
            if (k == conds.size()) {
                res_vi.add_tuple(tuple);
            }
        }
    }

    // process pattern optional for left view
    if (left->is_optional) {
        for (auto x : left_vi.cols) {
            // only span column is available for pattern optional, insert empty span as placeholder
            tuple[x.first] = Cell(Span(-1, -1, -1, -1));
        }

        for (int i=0; i<right_nrows; i++) {
            for (auto x : right_vi.cols)
                tuple[x.first] = right_vi.get_cell(x.first, i);
            res_vi.add_tuple(tuple);
        }
    }

    // process pattern optional for right view
    if (right->is_optional) {
        for (auto x : right_vi.cols) {
            // only span column is available for pattern optional, insert empty span as placeholder
            tuple[x.first] = Cell(Span(-1, -1, -1, -1));
        }

        for (int i=0; i<left_nrows; i++) {
            for (auto x : left_vi.cols)
                tuple[x.first] = left_vi.get_cell(x.first, i);
            res_vi.add_tuple(tuple);
        }
    }

    left_vi.clear();
    right_vi.clear();

    return res_vi;
}

void OpJoin::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Join\n", spaces, "");
    fprintf(stdout, "%*s+Conditions\n", spaces+tab, "");
    for (int i=0; i<conds.size(); i++) {
        fprintf(stdout, "%*s+[%d]\n", spaces+2*tab, "", i+1);
        conds[i]->dump(spaces+3*tab, tab);
    }
    left->dump(spaces+tab, tab);
    right->dump(spaces+tab, tab);
}
