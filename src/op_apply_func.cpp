#include "op_apply_func.h"

View OpApplyFunc::eval() {
    View vi = child->eval();
    Column *new_col = NULL;

    // create new column for function results
    switch (func->ret_type) {
        case PlanFunc::INT:
            new_col = new NumColumn(new_col_name);
            break;
        case PlanFunc::STRING:
            new_col = new StringColumn(new_col_name);
            break;
        case PlanFunc::SPAN:
            new_col = new SpanColumn(new_col_name);
            break;
        default:
            // should not go here
            break;
    }

    // TODO(ruanshihai): redundant argument transferring for tuple
    // apply function
    map<string, Cell> tuple;
    for (int i=0; i<vi.get_nrows(); i++) {
        for (auto x : vi.cols)
            tuple[x.first] = vi.get_cell(x.first, i);

        void *res = func->eval(tuple);
        switch (func->ret_type) {
            case PlanFunc::INT:
                ((NumColumn *)new_col)->nums.push_back(*(int *)res);
                delete((int *)res);
                break;
            case PlanFunc::STRING:
                ((StringColumn *)new_col)->strings.push_back(*(string *)res);
                delete((string *)res);
                break;
            case PlanFunc::SPAN:
                ((SpanColumn *)new_col)->spans.push_back(*(Span *)res);
                delete((Span *)res);
                break;
            default:
                // should not go here
                break;
        }
    }

    vi.cols[new_col_name] = new_col;
    return vi;
}


void OpApplyFunc::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+ApplyFunc\n", spaces, "");
    func->dump(spaces+tab, tab);
    fprintf(stdout, "%*s+NewColName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", new_col_name.c_str());
    child->dump(spaces+tab, tab);
}
