#include "op_scan.h"

View OpScan::eval() {
    View vi = store->get_view(view_name);

    // copy all columns
    for (auto it=vi.cols.begin(); it!=vi.cols.end(); it++) {
        switch (it->second->type) {
            case COLUMN_NUM:
                it->second = new NumColumn(*(NumColumn *)it->second);
                break;
            case COLUMN_STRING:
                it->second = new StringColumn(*(StringColumn *)it->second);
                break;
            case COLUMN_SPAN:
                it->second = new SpanColumn(*(SpanColumn *)it->second);
                break;
            default:
                // should not go here
                break;
        }
    }

    return vi;
}

void OpScan::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Scan\n", spaces, "");
    fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
}
