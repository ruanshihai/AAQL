#ifndef AQL_STORAGE_H
#define AQL_STORAGE_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

/*  ## 词性标注 ##
 *  类别        标记      iPOS    例子
 *  数词        m        18      120，40万，整，一位位
 *  人名        nr       24      谢顺桥
 *  汉语姓氏     nr1      25      赵，钱，孙，李
 *  汉语名字     nr2     26      智勇，让彩，官博，冬冬
 *  音译名      nrf      27      贝克汉姆，苏苏
 *  日语人名     nfj     28      小野，加藤，清水
 *  地名        ns      29      广州，厦门
 *  音译地名     nsf     30      波士顿，美国，日本，LONDON
 *  机构团体     nt      31      全国人大，国务院
 *  量词        q       37      辆，条，美元，张，位，条条
 *  时间        t       52      2012年，十月，十日，现在
 *  其它专有名词  nz     255     人民日报，人民网，奶茶
 */
enum POS_TAG {
    POS_NONE    = -1,
    POS_M       = 18,
    POS_NR      = 24,
    POS_NR1     = 25,
    POS_NR2     = 26,
    POS_NRF     = 27,
    POS_NRJ     = 28,
    POS_NS      = 29,
    POS_NSF     = 30,
    POS_NT      = 31,
    POS_Q       = 37,
    POS_T       = 52,
    POS_NZ      = 255
};

enum COLUMN_TYPE {
    COLUMN_NUM,
    COLUMN_STRING,
    COLUMN_SPAN
};

struct Word {
    Word() { iPOS = POS_NONE; }
    Word(int begin_, int end_, int iPOS_=POS_NONE) {
        begin = begin_; end = end_; iPOS = iPOS_;
    }

    int iPOS;
    int begin;
    int end;
};

struct Span {
    Span() {}
    Span(int begin_, int end_, int wbegin_, int wend_) {
        begin = begin_; end = end_; wbegin = wbegin_; wend = wend_;
    }

    int begin, end;
    int wbegin, wend;
};

struct Column {
    Column(string name_="", int type_=COLUMN_SPAN) { name = name_; type = type_; }
    virtual ~Column() {}

    int type;
    string name;
};

struct NumColumn : public Column {
    NumColumn(string name_="") : Column(name_, COLUMN_NUM) {}
    virtual ~NumColumn() {}

    vector<int> nums;
};

struct StringColumn : public Column {
    StringColumn(string name_="") : Column(name_, COLUMN_STRING) {}
    virtual ~StringColumn() {}

    vector<string> strings;
};

struct SpanColumn : public Column {
    SpanColumn(string name_="") : Column(name_, COLUMN_SPAN) {}
    virtual ~SpanColumn() {}

    vector<Span> spans;
};

struct Cell {
    enum CELL_TYPE {
        NONE, NUM, STRING, SPAN
    };
    Cell() : type(NONE) {}
    Cell(int n_) : type(NUM), n(n_) {}
    Cell(string word_) : type(STRING), word(word_) {}
    Cell(Span span_) : type(SPAN), span(span_) {}

    int type;
    int n;
    string word;
    Span span;
};

struct View {
    View(string name_="") { name = name_; }
    ~View() {}
    void clear() {
        for (auto x : cols)
            delete(x.second);
        cols.clear();
    }
    void add_tuple(map<string, Cell> &tuple) {
        for (auto x : cols) {
            switch (x.second->type) {
                case COLUMN_SPAN:
                    ((SpanColumn *)x.second)->spans.push_back(tuple[x.first].span);
                    break;
                case COLUMN_NUM:
                    ((NumColumn *)x.second)->nums.push_back(tuple[x.first].n);
                    break;
                case COLUMN_STRING:
                    ((StringColumn *)x.second)->strings.push_back(tuple[x.first].word);
                    break;
                default:
                    return;  // should not go here
            }
        }
    }
    int get_nrows() const {
        if (cols.empty()) {
            fprintf(stderr, "error: invalid view without any columns");
            exit(1);
        }

        Column *col = cols.begin()->second;
        switch (col->type) {
            case COLUMN_SPAN:
                return ((SpanColumn *)col)->spans.size();
            case COLUMN_NUM:
                return ((NumColumn *)col)->nums.size();
            case COLUMN_STRING:
                return ((StringColumn *)col)->strings.size();
            default:
                return -1;  // should not go here
        }
    }
    Cell get_cell(string col_name, int n) {
        Column * col = cols[col_name];
        switch (col->type) {
            case COLUMN_NUM:
                return Cell(((NumColumn *)col)->nums[n]);
            case COLUMN_STRING:
                return Cell(((StringColumn *)col)->strings[n]);
            case COLUMN_SPAN:
                return Cell(((SpanColumn *)col)->spans[n]);
            default:
                return Cell(); // should not go here
        }
    }

    string name;
    map<string, Column *> cols;
};

struct DictTreeNode {
    enum {
        WORD_SPLIT_CHAR=L'\uFFFF'
    };
    DictTreeNode(bool is_end_=false) { is_end = is_end_; }
    ~DictTreeNode() {
        for (auto x: childs)
            delete(x.second);
    }

    bool is_end;
    unordered_map<wchar_t, DictTreeNode *> childs;
};


class Storage {
public:
    wstring text;
    map<string, View> views;
    vector<Word> words;
    map<string, DictTreeNode *> dicts;

    Storage();
    ~Storage();
    void clear();
    int c2w(int ci);  // get word index from char index
    void add_view(View vi);
    View get_view(string view_name);

private:
    int bsearch(int low, int high, int ci);
};


#endif //AQL_STORAGE_H
