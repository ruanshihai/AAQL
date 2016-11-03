#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <cstring>
#include "lexer.h"
#include "nodes.h"

using namespace std;

class Parser {
public:
    NodeStmts *parser_tree;

	Parser(Lexer *lexer_);
    ~Parser();
    void program();
    void dump_tokens();
    void dump_parser_tree(int tab_=4);
private:
	Lexer *lexer;
	Token look;
    Token last;
    int tok_id;
    int group_id;  // help to capture pattern group number
    vector<Token> tokens;

    void move();
    void step_back();
    void match(int tag);
    void error(string msg);
    void error_tag(int tag);

    // top-down parse functions
    Node *aql_stmt();
    Node *empty_stmt();
    Node *create_stmt();
    Node *create_dict();
	Node *create_rule();
	Node *apply_clause();
    Node *prior_clause();
    Node *prior_atom();
    Node *with_clause();
    Node *when_clause();
    Node *conflict_atom();
    Node *clean_stmt();
    Node *create_view();
    Node *view_stmt();
    Node *sub_stmt();
    Node *output_stmt();
    Node *select_stmt();
    vector<Node *> select_list();
    Node *select_item();
    Node *arg();
    Node *func();
    Node *column();
    vector<Node *> from_list();
    Node *from_item();
    vector<Node *> where_clause();
    Node *extract_stmt();
    Node *extract_spec();
    Node *dict_spec();
    Node *regex_spec();
    vector<Node *> name_spec();
    vector<Node *> group_spec();
    Node *single_group();
    Node *pattern_spec();
    Node *pattern_expr();
    Node *pattern_alt();
    Node *pattern_seq();
    Node *pattern_opt();
    Node *pattern_repeat();
    Node *pattern_atom();
    Node *pattern_group();
};

#endif
