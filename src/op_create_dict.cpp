#include "op_create_dict.h"
#include "tokenizer.h"
#include "util.h"

void OpCreateDict::execute() {
    vector<wstring> w_words;

    if (dict_file == "") {
        // case 1: CREATE DICTIONARY ID AS ( word_list ) ;
        for (auto word : words)
            w_words.push_back(MBs2WCs(word.c_str()));

    } else {
        // case 2: CREATE DICTIONARY ID FROM TFILE STRING ;
        FILE *fp_dict = NULL;
        // TODO(ruanshihai): should handle any length
        const int MAX_WORD_LEN = 128;
        wchar_t buffer[MAX_WORD_LEN];
        string fpath;

        if (dict_file[0] == '/')
            fpath = dict_file;
        else
            fpath = base_dir+dict_file;

        fp_dict = fopen(fpath.c_str(), "r");
        if (!fp_dict) {
            fprintf(stderr, "error: can not open dictionary file: %s\n", fpath.c_str());
            exit(1);
        }

        while (fgetws(buffer, MAX_WORD_LEN, fp_dict)) {
            int len = wcslen(buffer);
            if (len>0 && buffer[len-1]==L'\n')
                buffer[--len] = L'\0';
            // blank line
            if (len == 0)
                continue;
            w_words.push_back(buffer);
        }
        fclose(fp_dict);
    }

    DictTreeNode *root = new DictTreeNode();
    DictTreeNode *ptr;
    vector<Word> split_words;

    // construct dictionary tree
    for (auto w_word : w_words) {
        ptr = root;
        split_words.clear();

        Tokenizer::nlpir_tokenize(w_word, split_words);
        for (int i=0; i<split_words.size(); i++) {
            for (int j=split_words[i].begin; j<split_words[i].end; j++) {
                if (ptr->childs.count(w_word[j])) {
                    ptr = ptr->childs[w_word[j]];
                } else {
                    ptr->childs[w_word[j]] = new DictTreeNode();
                    ptr = ptr->childs[w_word[j]];
                }
            }
            // dictionary matching is based on token boundary
            if (i != split_words.size()-1) {
                if (ptr->childs.count(DictTreeNode::WORD_SPLIT_CHAR) == 0)
                    ptr->childs[DictTreeNode::WORD_SPLIT_CHAR] = new DictTreeNode();
                ptr = ptr->childs[DictTreeNode::WORD_SPLIT_CHAR];
            }
        }
        ptr->is_end = true;
    }
    store->dicts[dict_name] = root;
}

void OpCreateDict::dump(int spaces, int tab) {
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
