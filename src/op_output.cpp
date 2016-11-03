#include "op_output.h"

void OpOutput::execute() {
    const View vi = store->get_view(view_name);
    int b, e, width;

    string display_name = (alias=="") ? view_name : alias;
    fprintf(stdout, "View: %s\n", display_name.c_str());

    // caculate rows number and cols number
    int n_cols = vi.cols.size();
    int n_rows = vi.get_nrows();

    // empty view
    if (n_rows==0 || n_cols==0) {
        fprintf(stdout, "Empty set\n\n");
        return;
    }

    // caculate each column's printing width
    int max_width[n_cols];
    int cur_col_id = 0;
    for (auto x : vi.cols) {
        max_width[cur_col_id] = x.first.size();
        Column *col = x.second;
        switch (col->type) {
            case COLUMN_NUM:
                for (int i=0; i<n_rows; i++) {
                    width = to_string(((NumColumn *)col)->nums[i]).size();
                    if (width > max_width[cur_col_id])
                        max_width[cur_col_id] = width;
                }
                break;
            case COLUMN_STRING:
                for (int i=0; i<n_rows; i++) {
                    width = ((StringColumn *)col)->strings[i].size();
                    if (width > max_width[cur_col_id])
                        max_width[cur_col_id] = width;
                }
                break;
            case COLUMN_SPAN:
                for (int i=0; i<n_rows; i++) {
                    b = ((SpanColumn *)col)->spans[i].begin;
                    e = ((SpanColumn *)col)->spans[i].end;
                    width = e-b+to_string(b).size()+to_string(e).size()+4;
                    if (width > max_width[cur_col_id])
                        max_width[cur_col_id] = width;
                }
                break;
            default:
                // should not go here
                break;
        }
        cur_col_id++;
    }

    // print split row
    fprintf(stdout, "+");
    for (int i=0; i< n_cols; i++) {
        for (int j=0; j< max_width[i]+2; j++)
            fprintf(stdout, "-");
        fprintf(stdout, "+");
    }
    fprintf(stdout, "\n");

    // print header
    cur_col_id = 0;
    for (auto x : vi.cols) {
        if (cur_col_id == 0)
            fprintf(stdout, "|");
        fprintf(stdout, " %-*s |", max_width[cur_col_id++], x.first.c_str());
    }
    fprintf(stdout, "\n");

    // print split row
    fprintf(stdout, "+");
    for (int i=0; i< n_cols; i++) {
        for (int j=0; j< max_width[i]+2; j++)
            fprintf(stdout, "-");
        fprintf(stdout, "+");
    }
    fprintf(stdout, "\n");

    // print content rows
    for (int i=0; i< n_rows; i++) {
        cur_col_id = 0;
        for (auto x : vi.cols) {
            if (cur_col_id == 0)
                fprintf(stdout, "|");
            Column *col = x.second;
            wstring content;
            switch (col->type) {
                case COLUMN_NUM:
                    fprintf(stdout, " %-*d |",
                            max_width[cur_col_id++], ((NumColumn *)col)->nums[i]);
                    break;
                case COLUMN_STRING:
                    fprintf(stdout, " %-*s |",
                            max_width[cur_col_id++],
                            ((StringColumn *)col)->strings[i].c_str());
                    break;
                case COLUMN_SPAN:
                    b = ((SpanColumn *)col)->spans[i].begin;
                    e = ((SpanColumn *)col)->spans[i].end;
                    // non-empty span
                    if (b >= 0)
                        content.assign(store->text.c_str()+b, e-b);
                    fprintf(stdout, " %S:(%d,%d)%*s |",
                            content.c_str(), b, e,
                            max_width[cur_col_id++]-(e-b+to_string(b).size()+to_string(e).size()+4), "");
                    break;
                default:
                    // should not go here
                    break;
            }
        }
        fprintf(stdout, "\n");
    }

    // print split row
    fprintf(stdout, "+");
    for (int i=0; i< n_cols; i++) {
        for (int j=0; j< max_width[i]+2; j++)
            fprintf(stdout, "-");
        fprintf(stdout, "+");
    }
    fprintf(stdout, "\n");

    // print statistical information
    fprintf(stdout, "%d rows in set\n\n", n_rows);
}

void OpOutput::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Output\n", spaces, "");
    fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
    if (alias != "") {
        fprintf(stdout, "%*s+Alias\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
    }
}
