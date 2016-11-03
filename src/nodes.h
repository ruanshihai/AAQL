#ifndef AQL_NODES_H
#define AQL_NODES_H


#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <cstdio>

using namespace std;

enum PARSER_NODE_TAG {
    NODE_NONE,
    NODE_STMTS,
    NODE_EMPTY_STMT,
    NODE_OUTPUT,
    NODE_CREATE_VIEW,
    NODE_CREATE_DICT,
    NODE_UNION,
    NODE_SELECT_STMT,
    NODE_SELECT_ITEM,
    NODE_FROM_ITEM,
    NODE_FUNC,
    NODE_ARG,
    NODE_EXTRACT_STMT,
    NODE_DICT_SPEC,
    NODE_REGEX_SPEC,
    NODE_SINGLIE_GROUP,
    NODE_COLUMN,
    NODE_PATTERN_SPEC,
    NODE_PATTERN_ALT,
    NODE_PATTERN_SEQ,
    NODE_PATTERN_OPT,
    NODE_PATTERN_REPEAT,
    NODE_PATTERN_ATOM,
    NODE_PATTERN_GROUP,
    NODE_CREATE_RULE,
    NODE_APPLY,
    NODE_PRIOR,
    NODE_WITH,
    NODE_CONFLICT,
    NODE_CLEAN,
    ARG_ALIAS
};

enum PARSER_NODE_ARG_TYPE {
    ARG_NONE,
    ARG_STAR,
    ARG_COLUMN,
    ARG_NUM,
    ARG_STRING,
    ARG_FUNC
};

enum PARSER_NODE_PATTERN_ATOM_TYPE {
    ATOM_NONE,
    ATOM_COLUMN,
    ATOM_TOKEN,
    ATOM_REGEX,
    ATOM_GROUP
};

/*
 * basic structure for parser tree
 */
struct Node {
    int tag;
    
    Node(int tag_=NODE_NONE) { tag = tag_; }
    virtual ~Node() {}
    virtual void dump(int spaces=0, int tab=4) {}
};

/*
 * aql_stmts → aql_stmt | aql_stmts aql_stmt
 * aql_stmt → create_stmt | output_stmt | empty_stmt
 * create_stmt → create_view | create_dict
 */
struct NodeStmts : public Node {
    vector<Node *> stmts;

    NodeStmts() : Node(NODE_STMTS) {}
    virtual ~NodeStmts() { for (auto x : stmts) delete(x); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+Program\n", spaces, "");
        for (int i=0; i<stmts.size(); i++) {
            fprintf(stdout, "%*s+[%d] Stmt\n", spaces+tab, "", i+1);
            stmts[i]->dump(spaces+2*tab, tab);
        }
    }
};

/*
 * empty_stmt → ;
 */
struct NodeEmptyStmt : public Node {
    NodeEmptyStmt() : Node(NODE_EMPTY_STMT) {}
    virtual ~NodeEmptyStmt() {}
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*sEmptyStmt\n", spaces, "");
    }
};

/*
 * output_stmt → OUTPUT VIEW ID alias ;
 * alias → AS ID | ε
 */
struct NodeOutput : public Node {
    string view_name;
    string alias;
    
    NodeOutput(string view_name_="", string alias_="") : Node(NODE_OUTPUT) {
        view_name = view_name_;
        alias = alias_;
    }
    virtual ~NodeOutput() {}
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+Output\n", spaces, "");
        fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
        if (alias != "") {
            fprintf(stdout, "%*s+Alias\n", spaces+tab, "");
            fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
        }
    }
};

/*
 * create_view → CREATE VIEW ID AS view_stmt ;
 */
struct NodeCreateView : public Node {
    string view_name;
    Node *view_stmt;
    
    NodeCreateView(string view_name_="", Node *view_=NULL) : Node(NODE_CREATE_VIEW) {
        view_name = view_name_;
        view_stmt = view_;
    }
    virtual ~NodeCreateView() { delete(view_stmt); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+CreateView\n", spaces, "");
        fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
        view_stmt->dump(spaces+tab, tab);
    }
};

/*
 * create_dict → CREATE DICTIONARY ID FROM TFILE STRING ;ao
 *             | CREATE DICTIONARY ID AS ( word_list ) ;
 */
struct NodeCreateDict : public Node {
    string dict_name;
    string dict_file;
    string base_dir;
    vector<string> words;
    
    NodeCreateDict(string base_dir_="", string dict_name_="", string dict_file_="")
            : Node(NODE_CREATE_DICT) {
        base_dir = base_dir_;
        dict_name = dict_name_;
        dict_file = dict_file_;
    }
    virtual ~NodeCreateDict() {}
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+CreateDict\n", spaces, "");
        fprintf(stdout, "%*s+DictName\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", dict_name.c_str());
        if (dict_file != "") {
            fprintf(stdout, "%*s+DictFile\n", spaces+tab, "");
            fprintf(stdout, "%*s'%s'\n", spaces+2*tab, "", dict_file.c_str());
        } else {
            fprintf(stdout, "%*s+Words\n", spaces+tab, "");
            for (int i=0; i<words.size(); i++) {
                fprintf(stdout, "%*s%s\n", spaces+2*tab, "", words[i].c_str());
                if (i < words.size()-1)
                    fprintf(stdout, ", ");
                else
                    fprintf(stdout, "\n");
            }
        }
    }
};

/*
 * create_rule		→	CREATE RULE ID AS apply_clause when_clause ;
 */
struct NodeCreateRule : public Node {
    string rule_name;
    Node *apply_func;
    Node *when_func;

    NodeCreateRule(string rule_name_="") : Node(NODE_CREATE_RULE) {
        rule_name = rule_name_;
    }

    virtual ~NodeCreateRule() {
        delete(apply_func);
        delete(when_func);
    }

    virtual void dump(int spaces=0, int tab=4) override {
        // TODO(songziwenhan)
        apply_func->dump();
        when_func->dump();
    }
};

/*
 * apply_clause     →   APPLY prior_list with_clause
 */
struct NodeApply : public Node {
    Node *prior_clause;
    Node *with_clause;

    NodeApply(Node *prior_=NULL, Node *with_=NULL) : Node(NODE_APPLY) {
        prior_clause = prior_;
        with_clause = with_;
    }

    virtual ~NodeApply() {
        delete(prior_clause);
        delete(with_clause);
    }

    virtual void dump(int spaces=0, int tab=4) override {
        // TODO(songziwenhan)
        prior_clause->dump();
        with_clause->dump();
    }
};


/*
 * prior_atom		→	pgd_func(...) to rx > ry
 */
struct NodePrior : public Node {
    Node *func;
    string row1, row2;

    NodePrior(Node *func_=NULL, string row1_="", string row2_="") : Node(NODE_PRIOR) {
        func = func_;
        row1 = row1_;
        row2 = row2_;
    }

    virtual ~NodePrior() {
        delete(func);
    }

    virtual void dump(int spaces=0, int tab=4) override {
        // TODO(songziwenhan)
    }
};

/*
 * with_clause      →   WITH
 */
struct NodeWith : public Node {
    map<string, string> rows;
    NodeWith() : Node(NODE_WITH) {
    }

    virtual ~NodeWith() {
    }

    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+With\n", spaces, "");
        fprintf(stdout, "%*s+Alias, size=%d\n", spaces+tab, "", rows.size());
    
            for (auto x = rows.begin(); x != rows.end(); x++) {
                fprintf(stdout, "%*s  %s in %s\n", spaces+2*tab, "", x->first.c_str(), x->second.c_str());
            }
    }
};

/*
 * conflict_atom     →   func
 */
struct NodeConflict : public Node {
    Node *func;

    NodeConflict(Node *func_=NULL) : Node(NODE_CONFLICT) {
        func = func_;
    }

    virtual ~NodeConflict() {
        delete(func);
    }

    virtual void dump(int spaces=0, int tab=4) override {
        // TODO(songziwenhan)
    }
};

/*
 * clean_stmt		→	CLEAN clean_atom
 */
struct NodeClean : public Node {
    vector<string> clean_list;

    NodeClean() : Node(NODE_CLEAN) {
    }

    virtual ~NodeClean() {
    }

    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+Clean\n", spaces, "");
        fprintf(stdout, "%*s+CleanList, size=%d\n", spaces+tab, "", clean_list.size());
    
            for (int i=0; i<clean_list.size(); i++) {
                fprintf(stdout, "%*s%s\n", spaces+2*tab, "", clean_list[i].c_str());
                if (i < clean_list.size()-1)
                    fprintf(stdout, ", ");
                else
                    fprintf(stdout, "\n");
            }
        
    }
};

/*
 * view_stmt → view_stmt UNION sub_stmt
 * sub_stmt → select_stmt | extract_stmt
 */
struct NodeUnion : public Node {
    Node *left;
    Node *right;

    NodeUnion(Node *left_=NULL, Node *right_=NULL) : Node(NODE_UNION) {
        left = left_;
        right = right_;
    }
    virtual ~NodeUnion() { delete(left); delete(right); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+NodeUnion\n", spaces, "");
        left->dump(spaces+tab, tab);
        right->dump(spaces+tab, tab);
    }
};

/*
 * select_stmt → SELECT select_list FROM from_list where_clause
 */
struct NodeSelectStmt : public Node {
    vector<Node *> select_list;
    vector<Node *> from_list;
    vector<Node *> condition_list;

    NodeSelectStmt() : Node(NODE_SELECT_STMT) {}
    virtual ~NodeSelectStmt() {
        for (auto x : select_list) delete(x);
        for (auto x : from_list) delete(x);
        for (auto x : condition_list) delete(x);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+SelectStmt\n", spaces, "");
        fprintf(stdout, "%*s+SelectList\n", spaces+tab, "");
        for (auto x : select_list)
            x->dump(spaces+2*tab, tab);
        fprintf(stdout, "%*s+FromList\n", spaces+tab, "");
        for (auto x : from_list)
            x->dump(spaces+2*tab, tab);
        if (!condition_list.empty()) {
            fprintf(stdout, "%*s+WhereClause\n", spaces + tab, "");
            for (auto x : condition_list)
                x->dump(spaces + 2*tab, tab);
        }
    }
};

/*
 * select_item → arg alias
 */
struct NodeSelectItem : public Node {
    Node *arg;
    string alias;

    NodeSelectItem(Node *arg_=NULL, string alias_="") : Node(NODE_SELECT_ITEM) {
        arg = arg_;
        alias =alias_;
    }
    virtual ~NodeSelectItem() { delete(arg); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+SelectItem\n", spaces, "");
        arg->dump(spaces+tab, tab);
        if (alias != "") {
            fprintf(stdout, "%*s+Alias\n", spaces+tab, "");
            fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
        }
    }
};

/*
 * from_item → ID ID | ( view_stmt ) ID
 */
struct NodeFromItem : public Node {
    string view_name;
    string alias;
    Node *view_stmt;

    NodeFromItem(string view_name_="", string alias_="") : Node(NODE_FROM_ITEM) {
        view_name = view_name_;
        alias = alias_;
        view_stmt = NULL;
    }
    virtual ~NodeFromItem() { delete(view_stmt); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+FromItem\n", spaces, "");
        if (view_stmt == NULL) {
            fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
            fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
        } else {
            view_stmt->dump(spaces + tab, tab);
        }
        if (alias != "") {
            fprintf(stdout, "%*s+Alias\n", spaces+tab, "");
            fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
        }
    }
};

/*
 * arg → * | column | NUM | STRING | func
 */
struct NodeArg : public Node {
    int type;
    int n;
    string word;
    string alias;
    Node *column;
    Node *func;

    NodeArg(int type_=ARG_NONE) : Node(NODE_ARG) {
        type = type_;
        column = NULL;
        n = 0;
        word = "";
        func = NULL;
    }
    virtual ~NodeArg() {
        if (type == ARG_COLUMN)
            delete(column);
        else if (type == ARG_FUNC)
            delete(func);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        switch (type) {
            case ARG_COLUMN:
                column->dump(spaces, tab);
                break;
            case ARG_NUM:
                fprintf(stdout, "%*s+NUM\n", spaces, "");
                fprintf(stdout, "%*s%d\n", spaces+tab, "", n);
                break;
            case ARG_STRING:
                fprintf(stdout, "%*s+STRING\n", spaces, "");
                fprintf(stdout, "%*s'%s'\n", spaces+tab, "", word.c_str());
                break;
            case ARG_FUNC:
                func->dump(spaces, tab);
                break;
            case ARG_STAR:
                fprintf(stdout, "*\n");
                break;
            default:
                // should not go here
                break;
        }
    }
};

/*
 * func → ID ( args )
 * args → arg | ε | args , arg
 */
struct NodeFunc : public Node {
    string func_name;
    vector<Node *> args;

    NodeFunc(string func_name_="") : Node(NODE_FUNC) { func_name = func_name_; }
    virtual ~NodeFunc() {
        for (auto x : args) delete(x);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+Func\n", spaces, "");
        fprintf(stdout, "%*s+FuncName\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", func_name.c_str());
        if (!args.empty()) {
            fprintf(stdout, "%*s+Args\n", spaces + tab, "");
            for (auto x : args)
                x->dump(spaces + 2*tab, tab);
        }
    }
};

/*
 * extract_stmt		→	EXTRACT select_list , extract_spec FROM from_list
 * extract_spec		→	dict_spec | regex_spec | pattern_spec
 */
struct NodeExtractStmt : public Node {
    vector<Node *> select_list;
    Node *extract_spec;
    vector<Node *> from_list;

    NodeExtractStmt() : Node(NODE_EXTRACT_STMT) { extract_spec = NULL; }
    virtual ~NodeExtractStmt() {
        for (auto x : select_list) delete(x);
        delete(extract_spec);
        for (auto x : from_list) delete(x);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+ExtractStmt\n", spaces, "");
        if (!select_list.empty()) {
            fprintf(stdout, "%*s+SelectList\n", spaces + tab, "");
            for (auto x : select_list)
                x->dump(spaces + 2 * tab, tab);
        }
        fprintf(stdout, "%*s+ExtractSpec\n", spaces + tab, "");
        extract_spec->dump(spaces + 2*tab, tab);
        fprintf(stdout, "%*s+FromList\n", spaces+tab, "");
        for (auto x : from_list)
            x->dump(spaces+2*tab, tab);
    }
};

/*
 * dict_spec → DICTIONARY ID on column AS ID
 */
struct NodeDictSpec : public Node {
    string dict_name;
    Node *on;
    string alias;

    NodeDictSpec(string dict_name_="") : Node(NODE_DICT_SPEC) {
        dict_name = dict_name_;
        on = NULL;
        alias = "";
    }
    virtual ~NodeDictSpec() {
        delete(on);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+DictSpec\n", spaces, "");
        fprintf(stdout, "%*s+DictName\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", dict_name.c_str());
        fprintf(stdout, "%*s+On\n", spaces+tab, "");
        on->dump(spaces+2*tab, tab);
        fprintf(stdout, "%*s+As\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
    }
};

/*
 * regex_spec → REGEX REG_EXPR ON column name_spec
 */
struct NodeRegexSpec : public Node {
    string reg_expr;
    Node *on;
    vector<Node *> group_list;

    NodeRegexSpec(string reg_expr_="", Node *on_=NULL) : Node(NODE_REGEX_SPEC) {
        reg_expr = reg_expr_;
        on = on_;
    }
    virtual ~NodeRegexSpec() {
        delete(on);
        for (auto x : group_list) delete(x);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+RegexSpec\n", spaces, "");
        fprintf(stdout, "%*s+RegExpr\n", spaces+tab, "");
        fprintf(stdout, "%*s%s\n", spaces+2*tab, "", reg_expr.c_str());
        fprintf(stdout, "%*s+On\n", spaces+tab, "");
        on->dump(spaces+2*tab, tab);
        fprintf(stdout, "%*s+GroupList\n", spaces+tab, "");
        for (auto x : group_list)
            x->dump(spaces+2*tab, tab);
    }
};

/*
 * single_group → GROUP NUM AS ID
 */
struct NodeSingleGroup : public Node {
    int n;
    string name;

    NodeSingleGroup(int n_=0, string name_="") : Node(NODE_SINGLIE_GROUP) {
        n = n_;
        name = name_;
    }
    virtual ~NodeSingleGroup() {}
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*sGroup %d => %s\n", spaces, "", n, name.c_str());
    }
};

/*
 * column → ID . ID
 */
struct NodeColumn : public Node {
    string view_name;
    string col_name;

    NodeColumn(string view_name_="", string col_name_="") : Node(NODE_COLUMN) {
        view_name = view_name_;
        col_name = col_name_;
    }
    virtual ~NodeColumn() {}
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+Column\n", spaces, "");
        fprintf(stdout, "%*s%s.%s\n", spaces+tab, "", view_name.c_str(), col_name.c_str());
    }
};

/*
 * pattern_spec → PATTERN pattern_expr name_spec
 */
struct NodePatternSpec : public Node {
    Node *pattern_expr;
    vector<Node *> group_list;

    NodePatternSpec(Node *pattern_expr_=NULL) : Node(NODE_PATTERN_SPEC) {
        pattern_expr = pattern_expr_;
    }
    virtual ~NodePatternSpec() {
        delete(pattern_expr);
        for (auto x : group_list) delete(x);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternSpec\n", spaces, "");
        fprintf(stdout, "%*s+PatternExpr\n", spaces+tab, "");
        pattern_expr->dump(spaces+2*tab, tab);
        fprintf(stdout, "%*s+GroupList\n", spaces+tab, "");
        for (auto x : group_list)
            x->dump(spaces+2*tab, tab);
    }
};

/*
 * pattern_alt → pattern_seq | pattern_alt '|' pattern_seq
 */
struct NodePatternAlt : public Node {
    Node *left;
    Node *right;

    NodePatternAlt(Node *left_=NULL, Node *right_=NULL) : Node(NODE_PATTERN_ALT) {
        left = left_;
        right = right_;
    }
    virtual ~NodePatternAlt() { delete(left); delete(right); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternAlt\n", spaces, "");
        left->dump(spaces+tab, tab);
        right->dump(spaces+tab, tab);
    }
};

/*
 * pattern_seq → pattern_opt | pattern_seq pattern_opt
 */
struct NodePatternSeq : public Node {
    Node *left;
    Node *right;

    NodePatternSeq(Node *left_=NULL, Node *right_=NULL) : Node(NODE_PATTERN_SEQ) {
        left = left_;
        right = right_;
    }
    virtual ~NodePatternSeq() { delete(left); delete(right); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternSeq\n", spaces, "");
        left->dump(spaces+tab, tab);
        right->dump(spaces+tab, tab);
    }
};

/*
 * pattern_opt → pattern_repeat | pattern_repeat ?
 */
struct NodePatternOpt : public Node {
    Node *child;

    NodePatternOpt(Node *child_=NULL) : Node(NODE_PATTERN_OPT) { child = child_; }
    virtual ~NodePatternOpt() { delete(child); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternOpt\n", spaces, "");
        child->dump(spaces+tab, tab);
    }
};

/*
 * pattern_repeat → pattern_atom | pattern_atom { NUM , NUM }
 */
struct NodePatternRepeat : public Node {
    int min;
    int max;
    Node *child;

    NodePatternRepeat(Node *child_=NULL, int min_=0, int max_=0) : Node(NODE_PATTERN_REPEAT) {
        child = child_;
        min = min_;
        max = max_;
    }
    virtual ~NodePatternRepeat() { delete(child); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternRepeat: {%d, %d}\n", spaces, "", min, max);
        child->dump(spaces+tab, tab);
    }
};

/*
 * pattern_atom → < column > | < TOKEN > | REG_EXPR | pattern_group
 */
struct NodePatternAtom : public Node {
    int type;
    string reg_expr;
    Node *column;
    Node *group;

    NodePatternAtom(int type_=ATOM_NONE) : Node(NODE_PATTERN_ATOM) {
        type = type_;
        reg_expr = "";
        column = NULL;
        group = NULL;
    }
    virtual ~NodePatternAtom() {
        if (type == ATOM_COLUMN)
            delete(column);
        else if (type == ATOM_GROUP)
            delete(group);
    }
    virtual void dump(int spaces=0, int tab=4) override {
        switch (type) {
            case ATOM_COLUMN:
                column->dump(spaces, tab);
                break;
            case ATOM_TOKEN:
                fprintf(stdout, "%*sToken\n", spaces, "");
                break;
            case ATOM_REGEX:
                fprintf(stdout, "%*s+Regex\n", spaces, "");
                fprintf(stdout, "%*s%s\n", spaces+tab, "", reg_expr.c_str());
                break;
            case ATOM_GROUP:
                group->dump(spaces, tab);
                break;
            default:
                // should not go here
                break;
        }
    }
};

/*
 * pattern_group → ( pattern_alt )
 */
struct NodePatternGroup : public Node {
    int n;
    Node *child;

    NodePatternGroup(int n_=0, Node *child_=NULL) : Node(NODE_PATTERN_GROUP) {
        n = n_;
        child = child_;
    }
    virtual ~NodePatternGroup() { delete(child); }
    virtual void dump(int spaces=0, int tab=4) override {
        fprintf(stdout, "%*s+PatternGroup: %d\n", spaces, "", n);
        child->dump(spaces+tab, tab);
    }
};


#endif //AQL_NODES_H
