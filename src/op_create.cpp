#include "op_create.h"

void OpCreate::execute() {
    View vi = child->eval();

    vi.name = view_name;
    // TODO(ruanshihai): may be redundant
    // rename all columns because intermediate result may not care about names
    for (auto x : vi.cols)
        x.second->name = x.first;
    store->add_view(vi);
}

void OpCreate::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+CreateView\n", spaces, "");
    fprintf(stdout, "%*s+ViewName\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", view_name.c_str());
    child->dump(spaces+tab, tab);
}
