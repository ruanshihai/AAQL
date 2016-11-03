#include "op_clean.h"


void OpClean::execute() {
    for (auto clean : clean_lists) {
        cleans[rules.find(clean)->second]->execute();
    }
}

void OpClean::dump(int spaces, int tab) {
}
