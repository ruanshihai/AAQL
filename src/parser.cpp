#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "parser.h"

Parser::Parser(Lexer *lexer_) {
	lexer = lexer_;
    parser_tree = NULL;
    tok_id = -1;
}

Parser::~Parser() {
    delete(parser_tree);
    parser_tree = NULL;
}

void Parser::move() {
    last = look;
	look = tokens[++tok_id];
}

void Parser::step_back() {
    if (tok_id < 0)
        return;
    look = last;
    if (tok_id >= 2)
        last = tokens[tok_id-2];
    tok_id--;
}

void Parser::match(int tag) {
	if (look.tag == tag) {
        move();
        return;
    }
    error_tag(tag);
}

void Parser::error(string msg) {
    fprintf(stderr, "%d:%d: error: syntax error: %s\n", look.lineno, look.colno, msg.c_str());
    if (look.tag != EOF)
        lexer->print_error_context(look.lineno, look.colno);
    exit(1);
}

void Parser::error_tag(int tag) {
    switch (tag) {
        case '.': error("expected '.'"); break;
        case '*': error("expected '*'"); break;
        case ',': error("expected ','"); break;
        case ';': error("expected ';'"); break;
        case '(': error("expected '('"); break;
        case ')': error("expected ')'"); break;
        case '{': error("expected '{'"); break;
        case '}': error("expected '}'"); break;
        case '<': error("expected '<'"); break;
        case '>': error("expected '>'"); break;
        case '?': error("expected '?'"); break;

        case CREATE: error("expected 'create'"); break;
        case OUTPUT: error("expected 'output'"); break;
        case SELECT: error("expected 'select'"); break;
        case EXTRACT: error("expected 'extract'"); break;
        case RETURN: error("expected 'return'"); break;
        case GROUP: error("expected 'group'"); break;
        case VIEW: error("expected 'view'"); break;
        case REGEX: error("expected 'regex'"); break;
        case PATTERN: error("expected 'pattern'"); break;
        case TOKEN: error("expected 'Token'"); break;
        case FROM: error("expected 'from'"); break;
        case AS: error("expected 'as'"); break;
        case ON: error("expected 'on'"); break;
        case AND: error("expected 'and'"); break;
        case DICTIONARY: error("dictionary 'create'"); break;
        case TFILE: error("expected 'file'"); break;
        case UNION: error("expected 'union'"); break;
        case WHERE: error("expected 'where'"); break;

        case NUM: error("expected a number"); break;
        case ID: error("expected an identifier"); break;
        case REG_EXPR: error("expected regular expression"); break;
        case STRING: error("expected a string"); break;

        default: error("unknown error"); break;
    }
}

/*
 * program			→	aql_stmts
 * aql_stmts		→	aql_stmt | aql_stmts aql_stmt
 */
void Parser::program() {
    lexer->get_all_tokens(tokens);
    move();

    parser_tree = new NodeStmts();
    while (look.tag != EOF) {
        Node *stmt = aql_stmt();
        parser_tree->stmts.push_back(stmt);
    }
}

/*
 * aql_stmt			→	create_stmt | output_stmt | empty_stmt
 * empty_stmt		→	;
 */
Node *Parser::aql_stmt() {
	if (look.tag == CREATE) {
        return create_stmt();
    } else if (look.tag == OUTPUT) {
        return output_stmt();
    } else if (look.tag == CLEAN) {
        return clean_stmt();
    } else if (look.tag == ';') {
        return empty_stmt();
    } else {
        error("expected 'create' or 'output' or 'clean'");
    }
    return NULL;
}

/*
 * empty_stmt		→	;
 */
Node *Parser::empty_stmt() {
    match(';');
    return new NodeEmptyStmt();
}

/*
 * create_stmt		→	create_view | create_dict
 */
Node *Parser::create_stmt() {
	match(CREATE);
	if (look.tag == DICTIONARY) {
        return create_dict();
    } else if (look.tag == VIEW) {
        return create_view();
    } else if (look.tag == RULE) {
        return create_rule();
    } else {
        error("expected 'view' or 'dictionary'");
    }
    return NULL;
}

/*
 * create_dict		→	CREATE DICTIONARY ID FROM TFILE STRING ;
 *					|	CREATE DICTIONARY ID AS ( word_list ) ;
 */
Node *Parser::create_dict() {
    NodeCreateDict *node_cd = new NodeCreateDict(lexer->fdir);
    match(DICTIONARY);
    match(ID);
    node_cd->dict_name = last.word;
    if (look.tag == FROM) {
        match(FROM);
        match(TFILE);
        match(STRING);
        node_cd->dict_file = last.word;
    } else if (look.tag == AS) {
        match(AS);
        match('(');
        match(STRING);
        node_cd->words.push_back(last.word);
        while (look.tag == ',') {
            match(',');
            match(STRING);
            node_cd->words.push_back(last.word);
        }
        match(')');
    } else {
        error("expected 'from' or 'as'");
    }
    match(';');
    return node_cd;
}

/*
 * create_rule		→	CREATE RULE ID AS apply_clause when_clause ;
 */
Node *Parser::create_rule() {
    NodeCreateRule *node_cr = new NodeCreateRule();
    match(RULE);
    match(ID);
    node_cr->rule_name = last.word;
    match(AS);
    node_cr->apply_func = apply_clause();
    if (look.tag == WHEN) {
        node_cr->when_func = when_clause();
    }
    match(';');
    return node_cr;
}

/*
 * apply_clause     →   APPLY prior_list with_clause
 */
Node *Parser::apply_clause() {
    NodeApply *node_ap = new NodeApply();
    match(APPLY);
    node_ap->prior_clause = prior_clause();
    node_ap->with_clause = with_clause();
    return node_ap;
}

/*
 * prior_atom		→   pgd_func(...) to rx > ry
 */
Node *Parser::prior_clause() {
    NodePrior *node_pa = new NodePrior();
    node_pa->func = func();
    match(TO);
    match(ID);
    node_pa->row1 = last.word;
    match('>');
    match(ID);
    node_pa->row2 = last.word;

    return node_pa;
}

/*
 * with_clause      →   WITH r1 in v1, r2 in v2 ...
 */
Node *Parser::with_clause() {
    NodeWith *node_wc = new NodeWith();
    match(WITH);
    match(ID);
    string temp_row = last.word;
    match(IN);
    match(ID);
    string temp_view = last.word;
    node_wc->rows.insert(std::pair<string, string>(temp_row, temp_view));
    while (look.tag == ',') {
        match(',');

        match(ID);
        temp_row = last.word;
        match(IN);
        match(ID);
        temp_view = last.word;
        node_wc->rows.insert(std::pair<string, string>(temp_row, temp_view));
        if (look.tag == WHEN || look.tag == ';') {
            break;
        }
    }

    return node_wc;
}

/*
 * when_clause      →   WHEN func() | ε
 */
Node *Parser::when_clause() {
    NodeConflict *node_cf = new NodeConflict();
    match(WHEN);
    node_cf->func = func();
    return node_cf;
}

/*
 * clean_stmt		→	CLEAN clean_atom
 */
Node *Parser::clean_stmt() {
    NodeClean *node_cs = new NodeClean();
    match(CLEAN);
    match(ID);
    string temp = last.word;
    node_cs->clean_list.push_back(temp);
    while (look.tag == ',') {
        match(',');
        match(ID);
        temp = last.word;
        node_cs->clean_list.push_back(temp);
    }
    match(';');
    return node_cs;
}


/*
 * create_view		→	CREATE VIEW ID AS view_stmt ;
 */
Node *Parser::create_view() {
    NodeCreateView *node_cv = new NodeCreateView();
    match(VIEW);
    match(ID);
    node_cv->view_name = last.word;
    match(AS);
    node_cv->view_stmt = view_stmt();
    match(';');
    return node_cv;
}

/*
 * view_stmt		→	sub_stmt | view_stmt UNION sub_stmt
 */
Node *Parser::view_stmt() {
    Node *stmt = sub_stmt();
    while (look.tag == UNION) {
        match(UNION);
        Node *right = sub_stmt();
        stmt = new NodeUnion(stmt, right);
    }
    return stmt;
}

/*
 * sub_stmt			→	select_stmt | extract_stmt
 */
Node *Parser::sub_stmt() {
    if (look.tag == SELECT) {
        return select_stmt();
    } else if (look.tag == EXTRACT) {
        return extract_stmt();
    } else {
        error("expected 'select' or 'extract'");
    }
    return NULL;
}

/*
 * output_stmt		→	OUTPUT VIEW ID alias ;
 * alias			→	AS ID | ε
 */
Node *Parser::output_stmt() {
    NodeOutput *node_o = new NodeOutput();
	match(OUTPUT);
	match(VIEW);
	match(ID);
    node_o->view_name = last.word;
	if (look.tag == AS) {
        match(AS);
        match(ID);
        node_o->alias = last.word;
    }
    match(';');
    return node_o;
}

/*
 * select_stmt		→	SELECT select_list FROM from_list where_clause
 */
Node *Parser::select_stmt() {
    NodeSelectStmt *node_ss = new NodeSelectStmt();
	match(SELECT);
	node_ss->select_list = select_list();
	match(FROM);
	node_ss->from_list = from_list();
    if (look.tag == WHERE)
        node_ss->condition_list = where_clause();
    return node_ss;
}

/*
 * select_list		→	select_item | select_list , select_item
 */
vector<Node *> Parser::select_list() {
    vector<Node *> slist;
    slist.push_back(select_item());
    while (look.tag == ',') {
        match(',');
        if (look.tag == PATTERN) {
            step_back();
            break;
        }
        slist.push_back(select_item());
    }
    return slist;
}

/*
 * select_item		→	arg alias
 */
Node *Parser::select_item() {
    NodeSelectItem *node_si = new NodeSelectItem();
    node_si->arg = arg();
    if (look.tag == AS) {
        match(AS);
        match(ID);
        node_si->alias = last.word;
    }
    return node_si;
}

/*
 * arg				→	* | column | NUM | STRING | func
 */
Node *Parser::arg() {
    NodeArg *node_arg = new NodeArg();
    switch (look.tag) {
        case '*':
            node_arg->type = ARG_STAR;
            break;
        case ID:
            match(ID);
            if (look.tag == '.') {
                node_arg->type = ARG_COLUMN;
                step_back();
                node_arg->column = column();
            } else if (look.tag == '(') {
                node_arg->type = ARG_FUNC;
                step_back();
                node_arg->func = func();
            } else {
                error("expected number, column or identifier");
            }
            break;
        case NUM:
            node_arg->type = ARG_NUM;
            match(NUM);
            node_arg->n = last.n;
            break;
        case STRING:
            node_arg->type = ARG_STRING;
            match(STRING);
            node_arg->word = last.word;
            break;
        default:
            error("expected '*', '<', number, string or identifier");
            break;
    }
    return node_arg;
}

/*
 * func			    →	ID ( args )
 * args			    →	arg | ε | args , arg
 */
Node *Parser::func() {
    NodeFunc *node_func = new NodeFunc();
    match(ID);
    node_func->func_name = last.word;
    match('(');
    if (look.tag != ')') {
        node_func->args.push_back(arg());
        while (look.tag == ',') {
            match(',');
            node_func->args.push_back(arg());
        }
    }
    match(')');
    return node_func;
}

/*
 * column			→	ID . ID
 */
Node *Parser::column() {
    NodeColumn *node_col = new NodeColumn();
    match(ID);
    node_col->view_name = last.word;
    match('.');
    match(ID);
    node_col->col_name = last.word;
    return node_col;
}

/*
 * from_list		→	from_item | from_list , from_item
 */
vector<Node *> Parser::from_list() {
    vector<Node *> flist;
    flist.push_back(from_item());
	while (look.tag == ',') {
		match(',');
        flist.push_back(from_item());
	}
    return flist;
}

/*
 * from_item		→	ID ID | ( view_stmt ) ID
 */
Node *Parser::from_item() {
    NodeFromItem *node_fi = new NodeFromItem();
    if (look.tag == ID) {
        match(ID);
        node_fi->view_name = last.word;
    } else if (look.tag == '(') {
        match('(');
        node_fi->view_stmt = view_stmt();
        match(')');
    } else {
        error("expected identifier or '('");
    }
    match(ID);
    node_fi->alias = last.word;
    return node_fi;
}

/*
 * where_clause		→	WHERE condition_list | ε
 * condition_list	→	func | condition_list AND func
 */
vector<Node *> Parser::where_clause() {
    vector<Node *> clist;
    match(WHERE);
    clist.push_back(func());
    while (look.tag == AND) {
        match(AND);
        clist.push_back(func());
    }
    return clist;
}

/*
 * extract_stmt		→	EXTRACT select_list , extract_spec FROM from_list
 */
Node *Parser::extract_stmt() {
    NodeExtractStmt *node_es = new NodeExtractStmt();
    match(EXTRACT);
    if (!(look.tag==DICTIONARY || look.tag==REGEX || look.tag==PATTERN)) {
        node_es->select_list = select_list();
        match(',');
    }
    node_es->extract_spec = extract_spec();
    match(FROM);
    node_es->from_list = from_list();
    return node_es;
}

/*
 * extract_spec		→	dict_spec | regex_spec | pattern_spec
 */
Node *Parser::extract_spec() {
    if (look.tag == DICTIONARY) {
        return dict_spec();
    } else if (look.tag == REGEX) {
        return regex_spec();
    } else if (look.tag == PATTERN) {
        return pattern_spec();
    } else {
        error("expected 'pattern', 'regex' or 'dictionary'");
    }
    return NULL;
}

/*
 * dict_spec		→	DICTIONARY ID on column AS ID
 */
Node *Parser::dict_spec() {
    NodeDictSpec *node_ds = new NodeDictSpec();
    match(DICTIONARY);
    match(ID);
    node_ds->dict_name = last.word;
    match(ON);
    node_ds->on = column();
    match(AS);
    match(ID);
    node_ds->alias = last.word;
    return node_ds;
}

/*
 * regex_spec		→	REGEX REG_EXPR ON column name_spec
 */
Node *Parser::regex_spec() {
    NodeRegexSpec *node_rs = new NodeRegexSpec();
	match(REGEX);
    match(REG_EXPR);
    node_rs->reg_expr = last.word;
	match(ON);
    node_rs->on = column();
	node_rs->group_list = name_spec();
    return node_rs;
}

/*
 * name_spec		→	AS ID | RETURN group_spec
 */
vector<Node *> Parser::name_spec() {
	if (look.tag == AS) {
        match(AS);
		match(ID);
        NodeSingleGroup *node_sg = new NodeSingleGroup(0, last.word);
        vector<Node *> glist;
        glist.push_back(node_sg);
        return glist;
	} else if (look.tag == RETURN) {
		match(RETURN);
		return group_spec();
	} else {
        error("expected 'as' or 'return'");
    }
    return vector<Node *>();
}

/*
 * group_spec		→	single_group | group_spec AND single_group
 */
vector<Node *> Parser::group_spec() {
    vector<Node *> glist;
	glist.push_back(single_group());
	while (look.tag == AND) {
		match(AND);
        glist.push_back(single_group());
	}
    return glist;
}

/*
 * single_group		→	GROUP NUM AS ID
 */
Node *Parser::single_group() {
    NodeSingleGroup *node_sg = new NodeSingleGroup();
	match(GROUP);
	match(NUM);
    node_sg->n = last.n;
	match(AS);
	match(ID);
    node_sg->name = last.word;
    return node_sg;
}

/*
 * pattern_spec		→	PATTERN pattern_expr name_spec
 */
Node *Parser::pattern_spec() {
    group_id = 0;
    NodePatternSpec *node_ps = new NodePatternSpec();
	match(PATTERN);
	node_ps->pattern_expr = pattern_expr();
	node_ps->group_list = name_spec();
    return node_ps;
}

/*
 * pattern_expr		→	pattern_alt
 */
Node *Parser::pattern_expr() {
	return pattern_alt();
}

/*
 * pattern_alt		→	pattern_seq | pattern_alt '|' pattern_seq
 */
Node *Parser::pattern_alt() {
    Node *alt = pattern_seq();
    while (look.tag == '|') {
        match('|');
        alt = new NodePatternAlt(alt, pattern_seq());
    }
    return alt;
}

/*
 * pattern_seq		→	pattern_opt | pattern_seq pattern_opt
 */
Node *Parser::pattern_seq() {
    Node *seq = pattern_opt();
    while (look.tag=='<' || look.tag=='(' || look.tag==REG_EXPR) {
        seq = new NodePatternSeq(seq, pattern_opt());
    }
    return seq;
}

/*
 * pattern_opt		→	pattern_repeat | pattern_repeat ?
 */
Node *Parser::pattern_opt() {
    Node *opt = pattern_repeat();
    if (look.tag == '?') {
        match('?');
        opt = new NodePatternOpt(opt);
    }
    return opt;
}

/*
 * pattern_repeat	→	pattern_atom | pattern_atom { NUM , NUM }
 */
Node *Parser::pattern_repeat() {
    Node *rep = pattern_atom();
    if (look.tag == '{') {
        match('{');
        match(NUM);
        int min = last.n;
        match(',');
        match(NUM);
        int max = last.n;
        match('}');
        rep = new NodePatternRepeat(rep, min, max);
    }
    return rep;
}

/*
 * pattern_atom		→	< column > | < TOKEN > | REG_EXPR | pattern_group
 */
Node *Parser::pattern_atom() {
    NodePatternAtom *atom = new NodePatternAtom();
	if (look.tag == '<') {
		match('<');
		if (look.tag == TOKEN) {
            match(TOKEN);
            atom->type = ATOM_TOKEN;
        } else {
            atom->type = ATOM_COLUMN;
            atom->column = column();
        }
		match('>');
	} else if (look.tag == REG_EXPR) {
        match(REG_EXPR);
        atom->type = ATOM_REGEX;
        atom->reg_expr = last.word;
	} else if (look.tag == '(') {
        atom->type = ATOM_GROUP;
        atom->group = pattern_group();
    } else {
        error("expected '<', '(' or '/'");
    }
    return atom;
}

/*
 * pattern_group	→	( pattern_alt )
 */
Node *Parser::pattern_group() {
	match('(');
    group_id++;
    Node *alt = pattern_alt();
	match(')');
    return new NodePatternGroup(group_id, alt);
}

void Parser::dump_tokens() {
    lexer->dump_tokens(tokens);
}

void Parser::dump_parser_tree(int tab_) {
    if (parser_tree == NULL) {
        fprintf(stderr, "error: parser tree is not built yet\n");
        exit(1);
    } else {
        parser_tree->dump(0, tab_);
        fprintf(stdout, "\n");
    }
}
