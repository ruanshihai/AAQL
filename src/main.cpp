#include <locale>
#include "lexer.h"
#include "parser.h"
#include "storage.h"
#include "aql_func.h"
#include "query_plan.h"
#include "extractor.h"
#include "NLPIR.h"

using namespace std;

AQLFunc * AQL_FUNC;

void usage() {
    fprintf(stderr,
            "Usage:\n"
            " AQL <AQL_filename> <TEXT_filename_or_dir>\n\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        usage();
        exit(1);
    }

    locale::global(locale("zh_CN.UTF-8"));
//    setlocale(LC_ALL, "zh_CN.UTF-8");

    if(!NLPIR_Init("../../AAQL", UTF8_CODE)) {
        fprintf(stderr, "error: NLPIR component init fails\n");
        return -1;
    }

    /* step 1: lexical analysis and syntactic analysis and generate parser tree */
    Lexer lexer(argv[1]);
    Parser parser(&lexer);
    parser.program();

    fprintf(stdout, "All tokens:\n");
    parser.dump_tokens();
    fprintf(stdout, "Parser tree:\n");
    parser.dump_parser_tree();

    /* step 2: generate query plan tree from parser tree */
    Storage store;
    QueryPlan plan(parser.parser_tree, &store);
    plan.generate();

    fprintf(stdout, "Query plan tree:\n");
    plan.dump_plan_tree();

    /* step 3: run extraction task for each document */
    AQL_FUNC = new AQLFunc(&store);
    Extractor extractor(&plan, &store, argv[2]);
    extractor.run();

    delete(AQL_FUNC);
    NLPIR_Exit();

    return 0;
}
