#include "tokenizer.h"
#include "util.h"

void Tokenizer::mix_tokenize(wstring &text_, vector<Word> &words_) {
    if (text_.size() == 0)
        return;

    int pos, lastPos;
    bool langFlag;

    pos = lastPos= 0;
    while (pos < text_.size()) {
        langFlag = text_[lastPos]>=L'\u0001' && text_[lastPos]<=L'\u007F';
        while (++pos < text_.size()) {
            bool tmpFlag = text_[pos]>=L'\u0001' && text_[pos]<=L'\u007F';
            if (tmpFlag != langFlag)
                break;
        }
        if (langFlag)
            en_tokenize(text_, lastPos, pos - 1, words_);
        else
            nlpir_tokenize(text_, lastPos, pos - 1, words_);
        lastPos = pos;
    }
}

void Tokenizer::en_tokenize(wstring &text_, vector<Word> &words_) {
    en_tokenize(text_, 0, text_.size()-1, words_);
}

void Tokenizer::en_tokenize(wstring &text_, int fromPos, int toPos, vector<Word> &words_) {
    const wchar_t whitespace[] = L" \t\r\n";
    int lastPos, curPos;

    lastPos = curPos = fromPos;
    do {
        if (curPos <= toPos) {
            if (text_[curPos] >= L'a' && text_[curPos] <= L'z'
                || text_[curPos] >= L'A' && text_[curPos] <= L'Z'
                || text_[curPos] >= L'0' && text_[curPos] <= L'9')
                continue;
        }

        if (curPos != lastPos)
            words_.push_back(Word(lastPos, curPos));
        if (wcschr(whitespace, text_[curPos])==NULL && curPos<=toPos)
            words_.push_back(Word(curPos, curPos+1));
        lastPos = curPos+1;
    } while (++curPos <= toPos+1);
}

void Tokenizer::nlpir_tokenize(wstring &text_, vector<Word> &words_) {
    nlpir_tokenize(text_, 0, text_.size()-1, words_);
}

void Tokenizer::nlpir_tokenize(wstring &text_, int fromPos, int toPos, vector<Word> &words_) {
    int nCount, low, high;
    const result_t *pResult;
    string byteText_;
    wstring tmpWord;

    // sends each line of text separated by LF to NLPIR
    low = high = fromPos;
    while (true) {
        while (high<=toPos && text_[high]!=L'\n')
            high++;
        if (low != high) {
            byteText_ = WCs2MBs(text_.substr(low, high-low).c_str());
            pResult = NLPIR_ParagraphProcessA(byteText_.c_str(), &nCount);
            for (int i=0; i<nCount; i++) {
                if (pResult[i].iPOS > 0) {
                    tmpWord = MBs2WCs(byteText_.substr(pResult[i].start, pResult[i].length).c_str());
                    while (text_[low] != tmpWord[0])
                        low++;
                    words_.push_back(Word(low, low+tmpWord.size(), pResult[i].iPOS));
                    low += tmpWord.size();
                }
            }
        }
        low = ++high;
        if (low > toPos)
            break;
    }
}
