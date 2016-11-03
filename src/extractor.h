#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <iostream>
#include <vector>
#include <map>
#include "util.h"
#include "storage.h"
#include "query_plan.h"
#include "tokenizer.h"

using namespace std;

#define BUILT_IN_VIEW_DOC       "Document"
#define BUILT_IN_VIEW_NE_PER    "Person"
#define BUILT_IN_VIEW_NE_LOC    "Location"
#define BUILT_IN_VIEW_NE_ORG    "Organization"
#define BUILT_IN_VIEW_NE_TIME   "Time"
#define BUILT_IN_COL            "text"

class Extractor {
public:
    Extractor(QueryPlan *query_plan_, Storage *store_, const char *fpath);
    ~Extractor();
    void run();

private:
    QueryPlan *query_plan;
    Storage *store;
    int cur_doc_id;
    string cur_doc_name;
    vector<string> docs;

    void get_docs(const char *fpath);
    int next_doc();
    void run_single_doc();
    void tokenize();
    void pre_process();
    void named_entity_recognize();
};

#endif
