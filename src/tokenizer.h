#ifndef AAQL_TOKENIZER_H
#define AAQL_TOKENIZER_H


#include <iostream>
#include "storage.h"
#include "NLPIR.h"

using namespace std;

class Tokenizer {
public:
    // mixture tokenizer of English and NLPIR
    static void mix_tokenize(wstring &text_, vector<Word> &words_);
    // tokenizer for English without text range
    static void en_tokenize(wstring &text_, vector<Word> &words_);
    // tokenizer for English with text range
    static void en_tokenize(wstring &text_, int fromPos, int toPos, vector<Word> &words_);
    // NLPIR tokenizer for English && Chinese without text range
    static void nlpir_tokenize(wstring &text_, vector<Word> &words_);
    // NLPIR tokenizer for English && Chinese with text range
    static void nlpir_tokenize(wstring &text_, int fromPos, int toPos, vector<Word> &words_);
};


#endif //AAQL_TOKENIZER_H
