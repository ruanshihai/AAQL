#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <map>

using namespace std;

enum LEXER_TOKEN_TAG {
    /* terminal tokens */
    NUM = 256,
    STRING,
    ID,
    REG_EXPR,

    /* keyword tokens */
    CREATE = 1000,
    OUTPUT,
    SELECT,
    EXTRACT,
    RETURN,
    GROUP,
    VIEW,
    REGEX,
    PATTERN,
    TOKEN,
    FROM,
    AS,
    ON,
    AND,
    DICTIONARY,
    TFILE,
    UNION,
    WHERE,
    RULE,
    APPLY,
    WITH,
    TRUE,
    CLEAN,
    WHEN,
    IN,
    TO
};

class Token {
public:
    int tag;
    int len;
    int n;
    string word;
    int lineno;
    int colno;
    
    Token() {}
    Token(int tag_, int len_, int lineno_, int colno_) {
        tag = tag_; len = len_; lineno = lineno_; colno = colno_;
    }
    Token(int tag_, int n_, int len_, int lineno_, int colno_) {
        tag = tag_; this->n = n_; len = len_; lineno = lineno_; colno = colno_;
    }
    Token(int tag_, string word_, int len_, int lineno_, int colno_) {
        tag = tag_; this->word = word_; len = len_; lineno = lineno_; colno = colno_;
    }
};

class Lexer {
public:
    int lineno;
    int colno;
    string fpath;
    string fdir;

    Lexer(const char *fpath_);
    ~Lexer();
    Token scan();
    void get_all_tokens(vector<Token> &tokens);
    void dump_tokens(vector<Token> &tokens);
    void print_error_context(int lineno, int colno);

private:
    FILE *fin;
    int peek;
    map<string, int> keywords;
    vector<string> lines;

    void reserve_keywords();
    void readch();
    int keyword_judge(string word);
    void error(int lineno, int colno, string msg);
};

#endif
