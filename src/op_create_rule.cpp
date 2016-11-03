#include "op_create_rule.h"

void OpCreateRule::execute() {
    string r1 = rows.find(row1)->first;
    string r2 = rows.find(row2)->first;
    
    auto src_vi1 = store->views.find(rows.find(row1)->second)->second;
    View res_vi1(src_vi1.name);
    auto src_vi2 = store->views.find(rows.find(row2)->second)->second;

    
    for (auto x : src_vi1.cols) {
        switch (x.second->type) {
            case COLUMN_NUM:
                res_vi1.cols[x.first] = new NumColumn(x.first);
                break;
            case COLUMN_STRING:
                res_vi1.cols[x.first] = new StringColumn(x.first);
                break;
            case COLUMN_SPAN:
                res_vi1.cols[x.first] = new SpanColumn(x.first);
                break;
            default:
                // should not go here
                break;
        }
    }
    map<string, Cell> tuple1;
    map<string, Cell> tuple2;

    /* multiple views */
    if (src_vi1.name != src_vi2.name) {
        View res_vi2(src_vi2.name);
        for (auto x : src_vi2.cols) {
            switch (x.second->type) {
                case COLUMN_NUM:
                    res_vi2.cols[x.first] = new NumColumn(x.first);
                    break;
                case COLUMN_STRING:
                    res_vi2.cols[x.first] = new StringColumn(x.first);
                    break;
                case COLUMN_SPAN:
                    res_vi2.cols[x.first] = new SpanColumn(x.first);
                    break;
                default:
                    // should not go here
                    break;
            }
        }

        int t1[((SpanColumn *)src_vi1.cols.begin()->second)->spans.size()] = {0};
        int t2[((SpanColumn *)src_vi2.cols.begin()->second)->spans.size()] = {0};
        for (int i=0; i<((SpanColumn *)src_vi1.cols.begin()->second)->spans.size(); i++) {
            if (t1[i] != 0)
                continue;
            for (auto x : src_vi1.cols)
                tuple1[r1+"."+x.first] = src_vi1.get_cell(x.first, i);
            for (int j=0; j<((SpanColumn *)src_vi2.cols.begin()->second)->spans.size(); j++) {
                for (auto y : src_vi2.cols)
                    tuple1[r2+"."+y.first] = src_vi2.get_cell(y.first, j);
                if(t2[j] == 0) {
                    bool *res = (bool *) when->eval(tuple1);
                    if (*res) {
                        delete (res);
                        bool *pri = (bool *) apply->eval(tuple1);
                        if (!*pri) {
                            delete (pri);
                            if (t1[i] == 0) {
                                t1[i] = 1;
                            }
                            break;
                        } else {
                            delete (pri);
                            t2[j] = 1;
                            continue;
                        }
                    } else {
                        delete (res);
                    }
                } else {
                    continue;
                }
            }
        }

        for (int i = 0; i < ((SpanColumn *)src_vi1.cols.begin()->second)->spans.size(); i++) {
            if (t1[i] == 0) {
                for (auto y : src_vi1.cols)
                    tuple2[y.first] = src_vi1.get_cell(y.first, i);

                res_vi1.add_tuple(tuple2);
            }
        }
        store->add_view(res_vi1);
        map<string, Cell> tuple;
        for (int i = 0; i < ((SpanColumn *)src_vi2.cols.begin()->second)->spans.size(); i++) {
            if (t2[i] == 0) {
                for (auto y : src_vi2.cols)
                    tuple[y.first] = src_vi2.get_cell(y.first, i);

                res_vi2.add_tuple(tuple);
            }
        }
        store->add_view(res_vi2);
    } else { /* single view */
        int t1[((SpanColumn *)src_vi1.cols.begin()->second)->spans.size()] = {0};
        for (int i=0; i<((SpanColumn *)src_vi1.cols.begin()->second)->spans.size(); i++) {
            if (t1[i] != 0)
                continue;
            for (auto x : src_vi1.cols)
                tuple1[r1+"."+x.first] = src_vi1.get_cell(x.first, i);
            for (int j = i+1; j < ((SpanColumn *) src_vi1.cols.begin()->second)->spans.size(); j++) {
                for (auto y : src_vi1.cols)
                    tuple1[r2+"."+y.first] = src_vi1.get_cell(y.first, j);

                if (t1[j] == 0) {
                    bool *res = (bool *) when->eval(tuple1);
                    if (*res) {
                        delete (res);
                        bool *pri = (bool *) apply->eval(tuple1);
                        if (!*pri) {
                            delete (pri);
                            if (t1[i] == 0) {
                                t1[i] = 1;
                            }
                            break;
                        } else {
                            delete (pri);
                            t1[j] = 1;
                            continue;
                        }
                    } else {
                        delete (res);
                    }
                } else {
                    break;
                }
            }

        }
        for (int i = 0; i < ((SpanColumn *)src_vi1.cols.begin()->second)->spans.size(); i++) {
            if (t1[i] == 0) {
                for (auto y : src_vi1.cols)
                    tuple2[y.first] = src_vi1.get_cell(y.first, i);

                res_vi1.add_tuple(tuple2);
            }
        }
        store->add_view(res_vi1);
    }
}

void OpCreateRule::dump(int spaces, int tab) {
    // TODO(songziwenhan)
}
