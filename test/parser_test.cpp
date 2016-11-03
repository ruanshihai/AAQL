#include "../src/lexer.h"
#include "../src/parser.h"

int main(int argc, char *argv[])
{
    Lexer lexer(argv[1]);
    Parser parser(&lexer);

    parser.program();

    fprintf(stdout, "All tokens:\n");
    parser.dump_tokens();

    fprintf(stdout, "Parser tree:\n");
    parser.dump_parser_tree();

    return 0;
}
