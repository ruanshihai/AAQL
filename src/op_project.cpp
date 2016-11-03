#include "op_project.h"

View OpProject::eval() {
    View src_vi = child->eval();

    if (action == PROJ_ADD_PREFIX) {
        View res_vi;
        for (auto col : src_vi.cols) {
            col.second->name = prefix + "." + col.first;
            res_vi.cols[prefix + "." + col.first] = col.second;
        }
        return res_vi;

    } else if (action == PROJ_SELECT_COLS) {
        View res_vi;
        for (auto col_name : cols) {
            res_vi.cols[col_name] = src_vi.cols[col_name];
            src_vi.cols.erase(col_name);
        }
        // free unnecessary columns, avoid memory leak
        src_vi.clear();
        return res_vi;

    } else if (action == PROJ_RENAME_COLS) {
        for (auto rename_pair : col_pairs) {
            src_vi.cols[rename_pair.first]->name = rename_pair.second;
            src_vi.cols[rename_pair.second] = src_vi.cols[rename_pair.first];
            src_vi.cols.erase(rename_pair.first);
        }
        return src_vi;

    } else if (action == PROJ_COPY_COLS) {
        for (auto copy_pair : col_pairs) {
            Column *new_col = NULL;
            switch (src_vi.cols[copy_pair.first]->type) {
                case COLUMN_SPAN:
                    new_col = new SpanColumn(copy_pair.second);
                    ((SpanColumn *)new_col)->spans = ((SpanColumn *)src_vi.cols[copy_pair.first])->spans;
                    src_vi.cols[copy_pair.second] = new_col;
                    break;
                case COLUMN_NUM:
                    new_col = new NumColumn(copy_pair.second);
                    ((NumColumn *)new_col)->nums = ((NumColumn *)src_vi.cols[copy_pair.first])->nums;
                    src_vi.cols[copy_pair.second] = new_col;
                    break;
                case COLUMN_STRING:
                    new_col = new StringColumn(copy_pair.second);
                    ((StringColumn *)new_col)->strings = ((StringColumn *)src_vi.cols[copy_pair.first])->strings;
                    src_vi.cols[copy_pair.second] = new_col;
                    break;
                default:
                    // should not go here
                    break;
            }
        }
        return src_vi;

    } else if (action == PROJ_NEW_NUM_COLUMN
               || action == PROJ_NEW_STRING_COLUMN) {
        int nrows = src_vi.get_nrows();

        if (action == PROJ_NEW_NUM_COLUMN) {
            NumColumn *num_col = new NumColumn(this->alias);
            num_col->nums.assign(nrows, this->n);
            src_vi.cols[this->alias] = num_col;

        } else if (action == PROJ_NEW_STRING_COLUMN) {
            StringColumn *str_col = new StringColumn(this->alias);
            str_col->strings.assign(nrows, this->word);
            src_vi.cols[this->alias] = str_col;
        }

        return src_vi;
    }

    return View();  // should not go here
}

void OpProject::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Project\n", spaces, "");

    if (action == PROJ_ADD_PREFIX) {
        fprintf(stdout, "%*s+AddPrefix\n", spaces+tab, "");
        fprintf(stdout, "%*s\"%s\"\n", spaces+2*tab, "", prefix.c_str());

    } else if (action == PROJ_SELECT_COLS) {
        fprintf(stdout, "%*s+SelectCols\n", spaces+tab, "");
        for (auto x : cols)
            fprintf(stdout, "%*s\"%s\"\n", spaces+2*tab, "", x.c_str());

    } else if (action == PROJ_RENAME_COLS) {
        fprintf(stdout, "%*s+RenameCols\n", spaces+tab, "");
        for (auto rename_pair : col_pairs)
            fprintf(stdout, "%*s\"%s\" => \"%s\"\n", spaces+2*tab, "",
                    rename_pair.first.c_str(), rename_pair.second.c_str());

    } else if (action == PROJ_COPY_COLS) {
        fprintf(stdout, "%*s+CopyCols\n", spaces+tab, "");
        for (auto copy_pair : col_pairs)
            fprintf(stdout, "%*s\"%s\" => \"%s\"\n", spaces+2*tab, "",
                    copy_pair.first.c_str(), copy_pair.second.c_str());

    } else if (action == PROJ_NEW_NUM_COLUMN) {
        fprintf(stdout, "%*s+NewNumColumn\n", spaces+tab, "");
        fprintf(stdout, "%*sNUM(%d) => \"%s\"\n",
                spaces+2*tab, "", this->n, alias.c_str());

    } else if (action == PROJ_NEW_STRING_COLUMN) {
        fprintf(stdout, "%*s+NewStringColumn\n", spaces+tab, "");
        fprintf(stdout, "%*sSTRING(%s) => \"%s\"\n",
                spaces+2*tab, "", this->word.c_str(), alias.c_str());

    }

    child->dump(spaces+tab, tab);
}
