#include "op_block.h"

View OpBlock::eval() {
    // group capture in pattern repeat is not available, only apply block operator to the leading column
    View src_vi = child->eval();
    View res_vi;
    SpanColumn *src_col = (SpanColumn *)src_vi.cols[child->leading_col_name];
    SpanColumn *res_col = new SpanColumn(new_col_name);
    res_vi.cols[new_col_name] = res_col;

    Span span;
    for (int j=0; j<=src_col->spans.size()-min; j++) {
        int count = 0;
        while (count < max) {
            if (count == 0) {
                if (min == 1)
                    res_col->spans.push_back(src_col->spans[j]);
            } else if (src_col->spans[j+count].wbegin-src_col->spans[j+count-1].wend==0
                       || src_col->spans[j+count].wbegin-src_col->spans[j+count-1].wend==1) {
                span.begin = src_col->spans[j].begin;
                span.end = src_col->spans[j+count].end;
                span.wbegin = src_col->spans[j].wbegin;
                span.wend = src_col->spans[j+count].wend;
                res_col->spans.push_back(span);
            } else {
                break;
            }
            count++;
        }
    }
    src_vi.clear();

    return res_vi;
}

void OpBlock::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Block\n", spaces, "");
    fprintf(stdout, "%*sMin: %d\n", spaces+tab, "", min);
    fprintf(stdout, "%*sMax: %d\n", spaces+tab, "", max);
    fprintf(stdout, "%*s+NewColName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", new_col_name.c_str());
    child->dump(spaces+tab, tab);
}
