#include "op_filter.h"

View OpFilter::eval() {
    View src_vi = child->eval();
    View res_vi(src_vi.name);

    // construct new view's structure
    for (auto x : src_vi.cols) {
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
    for (int i=0; i<((SpanColumn *)src_vi.cols.begin()->second)->spans.size(); i++) {
        for (auto x : src_vi.cols)
            tuple[x.first] = src_vi.get_cell(x.first, i);

        int k;
        for (k=0; k<filters.size(); k++) {
            bool *res = (bool *)filters[k]->eval(tuple);
            if (*res) {
                delete(res);
            } else {
                delete(res);
                break;
            }
        }

        // no filters or all filters evaluation are passed
        if (k == filters.size()) {
            res_vi.add_tuple(tuple);
        }
    }

    return res_vi;
}

void OpFilter::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Filter\n", spaces, "");
    fprintf(stdout, "%*s+Filters\n", spaces+tab, "");
    for (int i=0; i<filters.size(); i++) {
        fprintf(stdout, "%*s+[%d]\n", spaces+2*tab, "", i+1);
        filters[i]->dump(spaces+3*tab, tab);
    }
    child->dump(spaces+tab, tab);
}
