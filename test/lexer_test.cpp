#include <vector>
#include "../src/lexer.h"

int main(int argc, char *argv[])
{
    vector<Token> tokens;
    Lexer lexer(argv[1]);

    lexer.get_all_tokens(tokens);
    fprintf(stdout, "All tokens:\n");
    lexer.dump_tokens(tokens);

    return 0;
}
