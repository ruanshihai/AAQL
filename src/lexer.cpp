#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "lexer.h"

using namespace std;

Lexer::Lexer(const char *fpath_) {
    fpath = fpath_;
    int pos = fpath.find_last_of('/');
    if (pos > 0)
        fdir = fpath.substr(0, pos+1);
    else
        fdir = "./";

    fin = fopen(fpath.c_str(), "r");
    if (!fin) {
        fprintf(stderr, "error: can't open the file: %s\n", fpath.c_str());
        exit(1);
    }

    // TODO(ruanshihai): long lines should be considered
    char buffer[2048];
    while (fgets(buffer, 2048, fin)) {
        int len = strlen(buffer);
        if (len>0 && buffer[len-1]=='\n')
            buffer[len-1] = '\0';
        lines.push_back(buffer);
    }
    rewind(fin);

    lineno = 1;
    colno = -1;
    reserve_keywords();
    readch();
}

Lexer::~Lexer() {
    fclose(fin);
}

void Lexer::reserve_keywords() {
    keywords["create"] = CREATE;
    keywords["output"] = OUTPUT;
    keywords["select"] = SELECT;
    keywords["extract"] = EXTRACT;
    keywords["return"] = RETURN;
    keywords["group"] = GROUP;
    keywords["view"] = VIEW;
    keywords["regex"] = REGEX;
    keywords["pattern"] = PATTERN;
    keywords["Token"] = TOKEN;
    keywords["from"] = FROM;
    keywords["as"] = AS;
    keywords["on"] = ON;
    keywords["and"] = AND;
    keywords["dictionary"] = DICTIONARY;
    keywords["file"] = TFILE;
    keywords["union"] = UNION;
    keywords["where"] = WHERE;
    keywords["rule"] = RULE;
    keywords["apply"] = APPLY;
    keywords["to"] = TO;
    keywords["with"] = WITH;
    keywords["clean"] = CLEAN;
    keywords["when"] = WHEN;
    keywords["in"] = IN;
}

int Lexer::keyword_judge(string word) {
    auto it = keywords.find(word);
    if (it != keywords.end())
        return it->second;
    return -1;
}

void Lexer::readch() {
    peek = fgetc(fin);
    colno++;
}

void Lexer::get_all_tokens(vector<Token> &tokens) {
    Token tok;
    do {
        tok = scan();
        tokens.push_back(tok);
    } while (tok.tag != EOF);
}

void Lexer::error(int lineno, int colno, string msg) {
    fprintf(stderr, "%d:%d: error: %s\n", lineno, colno, msg.c_str());
    print_error_context(lineno, colno);
    exit(1);
}

void Lexer::print_error_context(int lineno, int colno) {
    fprintf(stderr, "%s\n", lines[lineno-1].c_str());
    fprintf(stderr, "%*s^\n", colno, "");
}

Token Lexer::scan() {
    /* whitespace */
    for ( ; ; readch()) {
        if (peek == '\t' || peek == ' ' || peek == '\r') {
            continue;
        } else if (peek == '\n') {
            lineno++;
            colno = -1;
        } else {
            break;
        }
    }

    int last_pos = colno;  // error start position

    /* line comment begin with "--" */
    if (peek == '-') {
        readch();
        if (peek != '-') {
            error(lineno, last_pos, string("unknown char: '-'"));
        }

        do {
            readch();
        } while (peek!=EOF && peek!='\n');

        if (peek != EOF) {
            lineno++;
            colno = -1;
            readch();
        }
        return scan();
    }

    /* seperator */
    switch (peek) {
        case '.':
            readch(); return Token('.', ".", 1, lineno, colno-1);
        case '*':
            readch(); return Token('.', ".", 1, lineno, colno-1);
        case ',':
            readch(); return Token(',', ",", 1, lineno, colno-1);
        case ';':
            readch(); return Token(';', ";", 1, lineno, colno-1);
        case '(':
            readch(); return Token('(', "(", 1, lineno, colno-1);
        case ')':
            readch(); return Token(')', ")", 1, lineno, colno-1);
        case '{':
            readch(); return Token('{', "{", 1, lineno, colno-1);
        case '}':
            readch(); return Token('}', "}", 1, lineno, colno-1);
        case '<':
            readch(); return Token('<', "<", 1, lineno, colno-1);
        case '>':
            readch(); return Token('>', ">", 1, lineno, colno-1);
        case '?':
            readch(); return Token('?', "?", 1, lineno, colno-1);
        case '|':
            readch(); return Token('|', "|", 1, lineno, colno-1);
        case '=':
            readch(); return Token('=', "=", 1, lineno, colno-1);
        default:
            break;
    }

    /* number */
    if (peek>='0' && peek<='9') {
        int n = 0, len = 0;
        do {
            n = 10*n+peek-'0';
            len++;
            readch();
        } while (peek>='0' && peek<='9');
        return Token(NUM, n, len, lineno, colno-len);
    }

    /* id */
    if (peek>='a' && peek<='z' || peek>='A' && peek<='Z' || peek=='_') {
        string word;
        do {
            word += peek;
            readch();
        } while (peek>='a' && peek<='z' || peek>='A' && peek<='Z'
                || peek>='0' && peek<='9'|| peek=='_');
        int tag = keyword_judge(word);
        if (tag != -1)
            return Token(tag, word, word.size(), lineno, colno-word.size());
        return Token(ID, word, word.size(), lineno, colno-word.size());
    }

    /* regular expression */
    if (peek == '/') {
        string word;
        char lastchar = 0;
        readch();
        while (peek && peek!='\n' && (peek!='/' || peek=='/' && lastchar=='\\')) {
            word += peek;
            lastchar = peek;
            readch();
        }
        if (peek==EOF || peek=='\n') {
            error(lineno, last_pos, "invalid regular expression");
        }
        readch();
        return Token(REG_EXPR, word, word.size()+2, lineno, colno-(word.size()+2));  // "+2" due to the enclosing '/'
    }

    /* string enclosed with single quotation marks */
    if (peek == '\'') {
        string word;
        readch();
        while (peek!=EOF && peek!='\n' && peek!='\'') {
            word += peek;
            readch();
        }
        if (peek==EOF || peek=='\n') {
            error(lineno, last_pos, "invalid string");
        }
        readch();
        return Token(STRING, word, word.size()+2, lineno, colno-(word.size()+2));  // "+2" due to the enclosing single quotation marks
    }

    /* EOF */
    if (peek == EOF)
        return Token(EOF, 0, lineno, colno);

    /* unknown character */
    error(lineno, last_pos, string("unknown char: '")+(char)peek+"'");
}

void Lexer::dump_tokens(vector<Token> &tokens) {
    int count = 0;

    for (auto tok : tokens) {
        switch (tok.tag) {
            case CREATE:
            case OUTPUT:
            case SELECT:
            case EXTRACT:
            case RETURN:
            case GROUP:
            case VIEW:
            case REGEX:
            case PATTERN:
            case TOKEN:
            case FROM:
            case AS:
            case ON:
            case AND:
            case DICTIONARY:
            case TFILE:
            case UNION:
            case WHERE:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: KEYWORD,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%s\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.word.c_str());
                break;
            case NUM:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: NUM,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%d\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.n);
                break;
            case ID:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: ID,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%s\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.word.c_str());
                break;
            case REG_EXPR:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: REG_EXPR,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%s\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.word.c_str());
                break;
            case STRING:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: STRING,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%s\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.word.c_str());
                break;
            case EOF:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: EOF,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%c\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.tag);
                break;
            default:
                fprintf(stdout, "[%d]\tTag: %d,\ttype: CHAR,\tlength: %d,\t"
                               "lineno: %d,\tcolno: %d,\tcontent: \"%c\"\n",
                       ++count, tok.tag, tok.len, tok.lineno, tok.colno, tok.tag);
                break;
        }
    }
    fprintf(stdout, "\n");
}
