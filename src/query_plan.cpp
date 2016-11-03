#include "query_plan.h"
#include <vector>
#include <cstring>
using namespace std;

QueryPlan::QueryPlan(Node *parser_tree_, Storage *store_) {
    parser_tree = parser_tree_;
    store = store_;
    tok_cond = NULL;
}

QueryPlan::~QueryPlan() {
    for (auto x : plan_trees)
        delete(x);
}

void QueryPlan::generate() {
    gen_stmts((NodeStmts *)parser_tree);
}

void QueryPlan::execute() {
    if (plan_trees.empty()) {
        fprintf(stderr, "error: query plan tree is not built yet\n");
        exit(1);
    }
    for (auto plan_tree : plan_trees) {
        // for all documents create dictionary operation only execute once
        if (plan_tree->type == OP_CREATE_DICT
            && store->dicts.find(((OpCreateDict *)plan_tree)->get_dict_name())!=store->dicts.end())
            continue;
        plan_tree->execute();
    }
}

void QueryPlan::gen_stmts(NodeStmts *nd_stmts) {
    Operator *plan = NULL;
    int counter = 0;
    for (auto nd_stmt : nd_stmts->stmts) {
        switch (nd_stmt->tag) {
            case NODE_EMPTY_STMT:
                break;
            case NODE_OUTPUT:
                plan = gen_output((NodeOutput *)nd_stmt);
                plan_trees.push_back(plan);
                break;
            case NODE_CREATE_VIEW:
                plan = gen_create_view((NodeCreateView *)nd_stmt);
                plan_trees.push_back(plan);
                break;
            case NODE_CREATE_DICT:
                plan = gen_create_dict((NodeCreateDict *)nd_stmt);
                plan_trees.push_back(plan);
                break;
            case NODE_CREATE_RULE:
                plan = gen_create_rule((NodeCreateRule *)nd_stmt);
                rules.insert(std::pair<string,int>(((OpCreateRule *)plan)->rule_name,counter++));
                cleanlist.push_back(plan);
                break;
            case NODE_CLEAN:
                plan = gen_clean((NodeClean *)nd_stmt);
                plan_trees.push_back(plan);
                break;
            default:
                // should not go here
                break;
        }
    }
}

Operator *QueryPlan::gen_output(NodeOutput *nd_output) {
    return new OpOutput(store, nd_output->view_name, nd_output->alias);
}

Operator *QueryPlan::gen_create_dict(NodeCreateDict *nd_cd) {
    return new OpCreateDict(store, nd_cd->dict_name, nd_cd->base_dir,
                            nd_cd->dict_file, nd_cd->words);
}

Operator *QueryPlan::gen_create_view(NodeCreateView *nd_cv) {
    Operator *child = gen_view_stmt(nd_cv->view_stmt);
    return new OpCreate(store, child, nd_cv->view_name);
}

Operator *QueryPlan::gen_view_stmt(Node *nd_vs) {
    if (nd_vs->tag == NODE_UNION) {
        Operator *left = gen_view_stmt(((NodeUnion *)nd_vs)->left);
        Operator *right = gen_view_stmt(((NodeUnion *)nd_vs)->right);
        return new OpUnion(left, right);
    }
    return gen_sub_stmt(nd_vs);
}

Operator *QueryPlan::gen_sub_stmt(Node *nd_ss) {
    if (nd_ss->tag == NODE_SELECT_STMT)
        return gen_select_stmt((NodeSelectStmt *)nd_ss);
    else if (nd_ss->tag == NODE_EXTRACT_STMT)
        return gen_extract_stmt((NodeExtractStmt *)nd_ss);
    else
        return NULL;  // should not go here
}

Operator *QueryPlan::gen_create_rule(NodeCreateRule *nd_cr) {

    PlanFunc *pgd = PlanFunc::make_func((NodeFunc *)(((NodePrior *)(((NodeApply *)(nd_cr->apply_func))->prior_clause))->func));
    PlanFunc *dc = PlanFunc::make_func((NodeFunc *)(((NodeConflict *)(nd_cr->when_func))->func));

    return new OpCreateRule(store, pgd, dc, (string)((NodePrior *)(((NodeApply *)(nd_cr->apply_func))->prior_clause))->row1, (string)((NodePrior *)(((NodeApply *)(nd_cr->apply_func))->prior_clause))->row2, (map<string, string>)((NodeWith *)((NodeApply *)(nd_cr->apply_func))->with_clause)->rows, nd_cr->rule_name);
}

Operator *QueryPlan::gen_clean(NodeClean *nd_cl) {
    return new OpClean(nd_cl->clean_list, cleanlist, rules);
}

Operator *QueryPlan::gen_select_stmt(NodeSelectStmt *nd_ss) {
    /* step 1: process from list */
    vector<NodeFromItem *> from_list;
    for (auto x : nd_ss->from_list)
        from_list.push_back((NodeFromItem *)x);
    map<string, Operator *> froms;
    froms = gen_from_list(from_list);

    /* step 2: process where clause */
    vector<PlanFunc *> multi_conds;  // conditions applied to more than one view
    map<PlanFunc *, set<string> > related_pairs;
    for (auto x : nd_ss->condition_list) {
        PlanFunc *cond = PlanFunc::make_func((NodeFunc *)x);
        cond->get_related_views(related_pairs[cond]);

        if (related_pairs[cond].size() == 1) {
            // condition applied to a single view
            vector<PlanFunc *> filters;
            filters.push_back(cond);
            string view_name = *related_pairs[cond].begin();
            froms[view_name] = new OpFilter(froms[view_name], filters);

        } else if (related_pairs[cond].size() > 1) {
            // condition applied to multiple views
            multi_conds.push_back(cond);

        } else {
            // TODO(ruanshihai): how about conditions applied to no views?
            // condition regarded as always true
            delete(cond);
        }
    }

    /* step 3: join views from left to right (order by froms) */
    Operator *plan = NULL;
    set<string> plan_views;  // views have been joined to query plan
    for (auto it=froms.begin(); it!=froms.end(); it++) {
        plan_views.insert(it->first);
        if (it == froms.begin()) {
            plan = it->second;
        } else {
            vector<PlanFunc *> join_conds;
            for (auto cond : multi_conds) {
                auto jt = related_pairs[cond].begin();
                for (; jt!=related_pairs[cond].end(); jt++) {
                    if (plan_views.count(*jt) == 0)
                        break;
                }
                if (jt == related_pairs[cond].end())
                    join_conds.push_back(cond);
            }
            plan = new OpJoin(plan, it->second, join_conds);
        }
    }

    /* step 4: process select list */
    vector<NodeSelectItem *> select_list;
    vector<string> placeholder;  // of no use
    for (auto x : nd_ss->select_list)
        select_list.push_back((NodeSelectItem *)x);
    return gen_select_list(select_list, plan, froms, placeholder);
}

Operator *QueryPlan::gen_extract_stmt(NodeExtractStmt *nd_es) {
    /* step 1: process from list */
    // TODO(ruanshihai): unused operators should be free manually
    vector<NodeFromItem *> from_list;
    for (auto x : nd_es->from_list)
        from_list.push_back((NodeFromItem *)x);
    map<string, Operator *> froms;
    froms = gen_from_list(from_list);

    /* step 2: process extraction specification */
    Operator *plan = NULL;
    // mark target columns selected by extraction specification and process them in step 3
    vector<string> sel_cols;
    if (nd_es->extract_spec->tag == NODE_DICT_SPEC) {
        plan = gen_dict_spec((NodeDictSpec *)nd_es->extract_spec, froms, sel_cols);
    } else if (nd_es->extract_spec->tag == NODE_REGEX_SPEC) {
        plan = gen_regex_spec((NodeRegexSpec *)nd_es->extract_spec, froms, sel_cols);
    } else if (nd_es->extract_spec->tag == NODE_PATTERN_SPEC) {
        plan = gen_pattern_spec((NodePatternSpec *)nd_es->extract_spec, froms, sel_cols);
    }

    /* step 3: process select list */
    vector<NodeSelectItem *> select_list;
    for (auto x : nd_es->select_list)
        select_list.push_back((NodeSelectItem *)x);
    return gen_select_list(select_list, plan, froms, sel_cols);
}

map<string, Operator *> QueryPlan::gen_from_list(vector<NodeFromItem *> &from_list) {
    map<string, Operator *> froms;
    Operator *plan = NULL;
    for (auto item : from_list) {
        if (item->view_stmt == NULL) {
            plan = new OpScan(store, item->view_name);
        } else {
            plan = gen_view_stmt(item->view_stmt);
        }
        plan = new OpProject(PROJ_ADD_PREFIX, plan, item->alias);
        froms[item->alias] = plan;
    }
    return froms;
}

Operator *QueryPlan::gen_regex_spec(NodeRegexSpec *nd_rs,
                                    map<string, Operator *> &froms,
                                    vector<string> &sel_cols) {
    // TODO(ruanshihai): ignore Document.text
    Operator *plan = new OpRegex(store, nd_rs->reg_expr);

    vector<NodeSingleGroup *> group_list;
    for (auto x : nd_rs->group_list)
        group_list.push_back((NodeSingleGroup *)x);
    plan = gen_group_list(group_list, plan, sel_cols);

    return plan;
}

Operator *QueryPlan::gen_dict_spec(NodeDictSpec *nd_ds,
                                   map<string, Operator *> &froms,
                                   vector<string> &sel_cols) {
    // TODO(ruanshihai): ignore Document.text
    sel_cols.push_back(nd_ds->alias);
    return new OpDictionary(store, nd_ds->dict_name, nd_ds->alias);
}

Operator *QueryPlan::gen_pattern_spec(NodePatternSpec *nd_ps,
                                      map<string, Operator *> &froms,
                                      vector<string> &sel_cols) {
    Operator *plan = gen_pattern_expr(nd_ps->pattern_expr, froms);

    // rename the whole pattern matching as group 0
    map<string, string> rename_cols;
    rename_cols[plan->leading_col_name] = Operator::get_group_name(0);
    plan = new OpProject(PROJ_RENAME_COLS, plan, rename_cols);

    vector<NodeSingleGroup *> group_list;
    for (auto x : nd_ps->group_list)
        group_list.push_back((NodeSingleGroup *)x);
    return gen_group_list(group_list, plan, sel_cols);
}

Operator *QueryPlan::gen_group_list(vector<NodeSingleGroup *> &group_list,
                                    Operator * child,
                                    vector<string> &sel_cols) {
    map<string, string> rename_cols;

    for (auto x : group_list) {
        rename_cols[Operator::get_group_name(x->n)] = x->name;
        sel_cols.push_back(x->name);
    }

    return new OpProject(PROJ_RENAME_COLS, child, rename_cols);
}

Operator *QueryPlan::gen_select_list(vector<NodeSelectItem *> &select_list,
                                     Operator *child,
                                     map<string, Operator *> &froms,
                                     vector<string> &sel_cols) {
    Operator *plan = child;
    map<string, string> rename_cols;

    for (auto item : select_list) {
        NodeArg *arg = (NodeArg *)item->arg;
        string alias = item->alias;

        if (arg->type == ARG_COLUMN) {
            if (alias == "")
                alias = ((NodeColumn *)arg->column)->col_name;
            rename_cols[((NodeColumn *)arg->column)->view_name+"."+((NodeColumn *)arg->column)->col_name] = alias;
            sel_cols.push_back(alias);

        } else if (arg->type == ARG_NUM) {
            plan = new OpProject(PROJ_NEW_NUM_COLUMN, plan, arg->n, alias);
            sel_cols.push_back(alias);

        } else if (arg->type == ARG_STRING) {
            plan = new OpProject(PROJ_NEW_STRING_COLUMN, plan, arg->word, alias);
            sel_cols.push_back(alias);

        } else if (arg->type == ARG_FUNC) {
            // TODO(ruanshihai): aggregation functions should be supported, such as min(), max(), count(*) etc
            PlanFunc *func = PlanFunc::make_func((NodeFunc *)arg->func);
            plan = new OpApplyFunc(plan, func, alias);
            sel_cols.push_back(alias);

        } else if (arg->type == ARG_STAR) {
            // TODO(ruanshihai): '*' should be supported
        }
    }

    if (!rename_cols.empty())
        plan = new OpProject(PROJ_RENAME_COLS, plan, rename_cols);
    plan = new OpProject(PROJ_SELECT_COLS, plan, sel_cols);

    return plan;
}

Operator *QueryPlan::gen_pattern_expr(Node *nd_pe,
                                      map<string, Operator *> &froms) {
    switch (nd_pe->tag) {
        case NODE_PATTERN_ALT:
            return gen_pattern_alt((NodePatternAlt *)nd_pe, froms);
        case NODE_PATTERN_SEQ:
            return gen_pattern_seq((NodePatternSeq *)nd_pe, froms);
        case NODE_PATTERN_OPT:
            return gen_pattern_opt((NodePatternOpt *)nd_pe, froms);
        case NODE_PATTERN_REPEAT:
            return gen_pattern_repeat((NodePatternRepeat *)nd_pe, froms);
        case NODE_PATTERN_ATOM:
            return gen_pattern_atom((NodePatternAtom *)nd_pe, froms);
        default:
            return NULL;  // should not go here
    }
}

Operator *QueryPlan::gen_pattern_alt(NodePatternAlt *nd_pa,
                                     map<string, Operator *> &froms) {
    // group capture in pattern alt is not available, only reserve the leading column
    Operator *left = gen_pattern_expr(nd_pa->left, froms);
    vector<string> lcols;
    string left_leading = left->leading_col_name;
    lcols.push_back(left_leading);
    left = new OpProject(PROJ_SELECT_COLS, left, lcols);

    Operator *right = gen_pattern_expr(nd_pa->right, froms);
    vector<string> rcols;
    string right_leading = right->leading_col_name;
    rcols.push_back(right_leading);
    right = new OpProject(PROJ_SELECT_COLS, right, rcols);

    // rename columns to the same name to make them available for join operator
    string alt_col_name = Operator::get_tmp_col_name();

    map<string, string> left_rename_cols;
    left_rename_cols[left_leading] = alt_col_name;
    left = new OpProject(PROJ_RENAME_COLS, left, left_rename_cols);

    map<string, string> right_rename_cols;
    right_rename_cols[right_leading] = alt_col_name;
    right = new OpProject(PROJ_RENAME_COLS, right, right_rename_cols);

    // union
    Operator *plan = new OpUnion(left, right);
    plan->leading_col_name = alt_col_name;

    return plan;
}

Operator *QueryPlan::gen_pattern_seq(NodePatternSeq *nd_ps,
                                     map<string, Operator *> &froms) {
    Operator *plan = NULL;
    Operator *left = gen_pattern_expr(nd_ps->left, froms);
    Operator *right = gen_pattern_expr(nd_ps->right, froms);

    // the right-hand side is <Token> or <Token>{m, n}
    if (right == NULL)
        return left;

    // step 1: join
    vector<PlanFunc *> conds;
    if (tok_cond != NULL) {
        // case 1: lhs <Token> rhs or lhs <Token>{m, n} rhs
        ((PlanArg *)tok_cond->args[0])->column = left->leading_col_name;
        ((PlanArg *)tok_cond->args[1])->column = right->leading_col_name;
        conds.push_back(tok_cond);
        plan = new OpJoin(left, right, conds);
        tok_cond = NULL;
    } else {
        // case 2: lhs rhs
        PlanFunc *cond = PlanFunc::make_follow_tok(left->leading_col_name,
                                                   right->leading_col_name, 0, 0);
        conds.push_back(cond);
        plan = new OpJoin(left, right, conds);
    }

    // step 2: create a new column of the whole matching
    PlanFunc *func = PlanFunc::make_combine_spans(left->leading_col_name,
                                                 right->leading_col_name);
    string new_col_name = Operator::get_tmp_col_name();
    plan = new OpApplyFunc(plan, func, new_col_name);
    plan->leading_col_name = new_col_name;
    plan->is_optional = (left->is_optional && right->is_optional);

    return plan;
}

Operator *QueryPlan::gen_pattern_opt(NodePatternOpt *nd_po,
                                     map<string, Operator *> &froms) {
    Operator *plan = gen_pattern_expr(nd_po->child, froms);
    plan->is_optional = true;

    return plan;
}

Operator *QueryPlan::gen_pattern_repeat(NodePatternRepeat *nd_pr,
                                        map<string, Operator *> &froms) {
    Operator *plan = gen_pattern_expr(nd_pr->child, froms);

    // the case of <Token>{m, n}
    if (plan == NULL) {
        ((NodeArg *)tok_cond->args[2])->n = nd_pr->min;
        ((NodeArg *)tok_cond->args[3])->n = nd_pr->max;
        return NULL;
    }

    string new_col_name = Operator::get_tmp_col_name();
    plan = new OpBlock(plan, nd_pr->min, nd_pr->max, new_col_name);
    plan->leading_col_name = new_col_name;

    return plan;
}

Operator *QueryPlan::gen_pattern_atom(NodePatternAtom *nd_pa,
                                      map<string, Operator *> &froms) {
    if (nd_pa->type == ATOM_TOKEN) {
        tok_cond = PlanFunc::make_follow_tok("", "", 1, 1);
        return NULL;

    } else if (nd_pa->type == ATOM_COLUMN) {
        NodeColumn *column = (NodeColumn *)nd_pa->column;
        string targ_name = column->view_name+"."+column->col_name;
        froms[column->view_name]->leading_col_name = targ_name;
        return froms[column->view_name];

    } else if (nd_pa->type == ATOM_REGEX) {
        Operator *plan = new OpRegex(store, nd_pa->reg_expr);

        // only reserve the whole matching group 0
        vector<string> sel_cols;
        sel_cols.push_back(Operator::get_group_name(0));
        plan = new OpProject(PROJ_SELECT_COLS, plan, sel_cols);

        // rename group 0 to avoid conflict with pattern capture group 0
        map<string, string> rename_cols;
        string new_name = Operator::get_tmp_col_name();
        rename_cols[Operator::get_group_name(0)] = new_name;
        plan = new OpProject(PROJ_RENAME_COLS, plan, rename_cols);
        plan->leading_col_name = new_name;

        return plan;

    } else if (nd_pa->type == ATOM_GROUP) {
        return gen_pattern_group((NodePatternGroup *)nd_pa->group, froms);
    }

    return NULL;  // should not go here
}

Operator *QueryPlan::gen_pattern_group(NodePatternGroup *nd_pg,
                                       map<string, Operator *> &froms) {
    Operator *plan = gen_pattern_expr(nd_pg->child, froms);

    // copy the leading column as the result of group capture
    map<string, string> copy_cols;
    copy_cols[plan->leading_col_name] = Operator::get_group_name(nd_pg->n);
    plan = new OpProject(PROJ_COPY_COLS, plan, copy_cols);
    plan->leading_col_name = Operator::get_group_name(nd_pg->n);

    return plan;
}

void QueryPlan::dump_plan_tree(int tab) {
    if (plan_trees.empty()) {
        fprintf(stderr, "error: empty tree or query plan tree is not built yet\n");
        exit(1);
    } else {
        fprintf(stdout, "%*s+QueryPlanTree\n", 0, "");
        for (int i=0; i<plan_trees.size(); i++) {
            fprintf(stdout, "%*s+[%d] Stmt\n", tab, "", i+1);
            plan_trees[i]->dump(2*tab, tab);
        }
        fprintf(stdout, "\n");
    }
}
