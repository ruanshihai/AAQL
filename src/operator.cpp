#include "operator.h"

string Operator::get_tmp_col_name() {
    static int count = 0;
    return "__AQL_TMP_COL_"+to_string(++count);
}

string Operator::get_group_name(int n) {
    return "__AQL_GROUP_"+to_string(n);
}
