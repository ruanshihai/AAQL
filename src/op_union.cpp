#include "op_union.h"

View OpUnion::eval() {
    View left_vi = left->eval();
    View right_vi = right->eval();

    if (left_vi.cols.size() != right_vi.cols.size()) {
        fprintf(stderr, "error: invalid union operator (unequal length)");
        exit(1);
    }
    for (auto x : left_vi.cols) {
        if (right_vi.cols.find(x.first) == right_vi.cols.end()) {
            fprintf(stderr, "error: invalid union operator (different columns)");
            exit(1);
        }
    }

    for (auto col : left_vi.cols) {
        switch (col.second->type) {
            case COLUMN_NUM:
                ((NumColumn *)col.second)->nums.insert(
                        ((NumColumn *)col.second)->nums.end(),
                        ((NumColumn *)right_vi.cols[col.first])->nums.begin(),
                        ((NumColumn *)right_vi.cols[col.first])->nums.end());
                break;
            case COLUMN_STRING:
                ((StringColumn *)col.second)->strings.insert(
                        ((StringColumn *)col.second)->strings.end(),
                        ((StringColumn *)right_vi.cols[col.first])->strings.begin(),
                        ((StringColumn *)right_vi.cols[col.first])->strings.end());
                break;
            case COLUMN_SPAN:
                ((SpanColumn *)col.second)->spans.insert(
                        ((SpanColumn *)col.second)->spans.end(),
                        ((SpanColumn *)right_vi.cols[col.first])->spans.begin(),
                        ((SpanColumn *)right_vi.cols[col.first])->spans.end());
                break;
            default:
                // should not go here
                break;
        }
    }
    right_vi.clear();

    return left_vi;
}

void OpUnion::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Union\n", spaces, "");
    left->dump(spaces+tab, tab);
    right->dump(spaces+tab, tab);
}
