#include "op_dictionary.h"

View OpDictionary::eval() {
    View vi;
    SpanColumn *col = new SpanColumn(alias);
    DictTreeNode *root = store->dicts[dict_name];
    vi.cols[alias] = col;

    // dictionary matching starts from each token boundary
    for (int from_word_id=0; from_word_id < store->words.size(); from_word_id++) {
        DictTreeNode *ptr = root;
        int cur_word_id = from_word_id;

        while (cur_word_id < store->words.size()) {
            bool fail = false;
            for (int i=store->words[cur_word_id].begin; i<store->words[cur_word_id].end; i++) {
                if (ptr->childs.count(store->text[i])) {
                    ptr = ptr->childs[store->text[i]];
                } else {
                    fail = true;
                    break;
                }
            }
            if (fail)
                break;
            if (ptr->is_end) {
                Span span;
                span.begin = store->words[from_word_id].begin;
                span.end = store->words[cur_word_id].end;
                span.wbegin = store->c2w(span.begin);
                span.wend = store->c2w(span.end-1);
                col->spans.push_back(span);
            }
            if (ptr->childs.count(DictTreeNode::WORD_SPLIT_CHAR) == 0)
                break;
            ptr = ptr->childs[DictTreeNode::WORD_SPLIT_CHAR];
            cur_word_id++;
        }
    }

    return vi;
}

void OpDictionary::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Dictionary\n", spaces, "");
    fprintf(stdout, "%*s+DictName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", dict_name.c_str());
    fprintf(stdout, "%*s+As\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", alias.c_str());
}
