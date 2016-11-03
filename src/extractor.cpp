#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <dirent.h>
#include <sys/stat.h>
#include "extractor.h"
#include "NLPIR.h"

Extractor::Extractor(QueryPlan *query_plan_, Storage *store_, const char *fpath) {
    query_plan = query_plan_;
    store = store_;
    get_docs(fpath);
}

Extractor::~Extractor() {

}

void Extractor::get_docs(const char *fpath) {
    struct stat st;
    DIR* dp;
    struct dirent* dirp;

    cur_doc_id = 0;
    if(stat(fpath, &st) == -1) {
        fprintf(stderr, "error: no such file or directory\n");
        exit(1);
    }
    if (!S_ISDIR(st.st_mode)) {
        docs.push_back(fpath);
    } else {
        if((dp = opendir(fpath)) == NULL) {
            fprintf(stderr, "error: can not open the directory\n");;
            return;
        }
        while((dirp=readdir(dp)) != NULL) {
            if(dirp->d_name[0] == '.')
                continue;
            string full_name = fpath;
            if (full_name[full_name.size()-1] == '/')
                full_name += dirp->d_name;
            else
                full_name = full_name +"/"+dirp->d_name;
            docs.push_back(full_name);
        }
    }
    sort(docs.begin(), docs.end());
}

void Extractor::run() {
    while (next_doc()) {
        fprintf(stdout, "Processing %s\n", cur_doc_name.c_str());
        run_single_doc();
        store->clear();
    }
}

int Extractor::next_doc() {
    if (cur_doc_id >= docs.size())
        return 0;

    FILE *fp_doc;
    long len;
    char *buffer;
    long result;

    cur_doc_name = docs[cur_doc_id++];
    fp_doc = fopen(cur_doc_name.c_str(), "rb");
    if (fp_doc == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    fseek(fp_doc, 0, SEEK_END);
    len = ftell(fp_doc);
    rewind(fp_doc);

    buffer = (char*)malloc(sizeof(char)*(len +1));
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        exit(2);
    }

    result = fread(buffer,1, len, fp_doc);
    if (result != len) {
        fputs("Reading error", stderr);
        exit(3);
    }
    buffer[len] = '\0';
    fclose(fp_doc);

    store->text = MBs2WCs(buffer);
    free(buffer);

    return 1;
}

void Extractor::run_single_doc() {
    tokenize();
    pre_process();
    query_plan->execute();
}

void Extractor::tokenize() {
    Tokenizer::nlpir_tokenize(store->text, store->words);
}

void Extractor::pre_process() {
    // add built-in view "Document"
    View doc(BUILT_IN_VIEW_DOC);
    SpanColumn *col = new SpanColumn(BUILT_IN_COL);
    col->spans.push_back(Span(0, store->text.size(), 0, store->words.size()-1));
    doc.cols[BUILT_IN_COL] = col;
    store->add_view(doc);

    named_entity_recognize();
}

void Extractor::named_entity_recognize() {
    View per(BUILT_IN_VIEW_NE_PER);
    View loc(BUILT_IN_VIEW_NE_LOC);
    View org(BUILT_IN_VIEW_NE_ORG);
    View time(BUILT_IN_VIEW_NE_TIME);

    SpanColumn *col_per = new SpanColumn(BUILT_IN_COL);
    SpanColumn *col_loc = new SpanColumn(BUILT_IN_COL);
    SpanColumn *col_org = new SpanColumn(BUILT_IN_COL);
    SpanColumn *col_time = new SpanColumn(BUILT_IN_COL);

    int lastPos;
    for (int pos=0; pos<store->words.size(); pos++) {
        lastPos = pos;
        switch (store->words[pos].iPOS) {
            case POS_NR:
            /*case POS_NR1:*/
            case POS_NR2:
            case POS_NRF:
            case POS_NRJ:
                while (pos+1<store->words.size() && store->words[pos+1].iPOS==store->words[pos].iPOS)
                    pos++;
                col_per->spans.push_back(
                                Span(store->words[lastPos].begin,
                                     store->words[pos].end, lastPos, pos));
                break;
            case POS_NS:
            case POS_NSF:
                while (pos+1<store->words.size() && store->words[pos+1].iPOS==store->words[pos].iPOS)
                    pos++;
                col_loc->spans.push_back(
                                Span(store->words[lastPos].begin,
                                     store->words[pos].end, lastPos, pos));
                break;
            case POS_NT:
                while (pos+1<store->words.size() && store->words[pos+1].iPOS==store->words[pos].iPOS)
                    pos++;
                col_org->spans.push_back(
                                Span(store->words[lastPos].begin,
                                     store->words[pos].end, lastPos, pos));
                break;
            case POS_T:
                while (pos+1<store->words.size() && store->words[pos+1].iPOS==store->words[pos].iPOS)
                    pos++;
                col_time->spans.push_back(
                                Span(store->words[lastPos].begin,
                                     store->words[pos].end, lastPos, pos));
                break;
            default:
                break;
        }
    }

    per.cols[BUILT_IN_COL] = col_per;
    loc.cols[BUILT_IN_COL] = col_loc;
    org.cols[BUILT_IN_COL] = col_org;
    time.cols[BUILT_IN_COL] = col_time;

    store->add_view(per);
    store->add_view(loc);
    store->add_view(org);
    store->add_view(time);
}
