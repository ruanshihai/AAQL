#ifndef AQL_QUERY_PLAN_H
#define AQL_QUERY_PLAN_H

#include "nodes.h"
#include "storage.h"
#include "operator.h"
#include "op_output.h"
#include "op_create.h"
#include "op_create_rule.h"
#include "op_clean.h"
#include "op_scan.h"
#include "op_union.h"
#include "op_join.h"
#include "op_filter.h"
#include "op_project.h"
#include "op_apply_func.h"
#include "op_regex.h"
#include "op_block.h"
#include "op_dictionary.h"
#include "op_create_dict.h"

using namespace std;

class QueryPlan {
public:
    vector<Operator *> plan_trees;
    map<string, int> rules;
    vector<Operator *> cleanlist;
    QueryPlan(Node *parser_tree_, Storage *store_);
    ~QueryPlan();
    void generate();
    void execute();
    void dump_plan_tree(int tab=4);

private:
    Node *parser_tree;
    Storage *store;
    PlanFunc *tok_cond;  // condition for Join operator in pattern matching
    

    // functions for query plan generating
    void gen_stmts(NodeStmts *nd_stmts);
    Operator *gen_output(NodeOutput *nd_output);
    Operator *gen_create_dict(NodeCreateDict *nd_cd);
    Operator *gen_create_view(NodeCreateView *nd_cv);
    Operator *gen_create_rule(NodeCreateRule *nd_cr);
    Operator *gen_clean(NodeClean *nd_cl);
    Operator *gen_view_stmt(Node *nd_vs);
    Operator *gen_sub_stmt(Node* nd_ss);
    Operator *gen_select_stmt(NodeSelectStmt *nd_ss);
    Operator *gen_extract_stmt(NodeExtractStmt *nd_es);
    map<string, Operator *> gen_from_list(vector<NodeFromItem *> &from_list);
    Operator *gen_regex_spec(NodeRegexSpec *nd_rs,
                             map<string, Operator *> &froms,
                             vector<string> &sel_cols);
    Operator *gen_dict_spec(NodeDictSpec *nd_ds,
                            map<string, Operator *> &froms,
                            vector<string> &sel_cols);
    Operator *gen_pattern_spec(NodePatternSpec *nd_ps,
                               map<string, Operator *> &froms,
                               vector<string> &sel_cols);
    Operator *gen_group_list(vector<NodeSingleGroup *> &group_list,
                             Operator *child,
                             vector<string> &sel_cols);
    Operator *gen_select_list(vector<NodeSelectItem *> &select_list,
                              Operator *child,
                              map<string, Operator *> &froms,
                              vector<string> &sel_cols);
    Operator *gen_pattern_expr(Node *nd_pe,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_alt(NodePatternAlt *nd_pa,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_seq(NodePatternSeq *nd_ps,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_opt(NodePatternOpt *nd_po,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_repeat(NodePatternRepeat *nd_pr,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_atom(NodePatternAtom *nd_pa,
                               map<string, Operator *> &froms);
    Operator *gen_pattern_group(NodePatternGroup *nd_pg,
                               map<string, Operator *> &froms);
};


#endif //AQL_QUERY_PLAN_H
