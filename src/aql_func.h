#ifndef AAQL_AQL_FUNC_H
#define AAQL_AQL_FUNC_H


#include "storage.h"

class AQLFunc {
public:
    AQLFunc(Storage *store_) { store = store_; }

    // TODO(ruanshihai): monitor function additions, deletions and modifications
    static bool FollowTok(Span lhs, Span rhs, int min, int max);
    static Span CombineSpans(Span lhs, Span rhs);
    static bool True();
    static bool Or(bool lhs, bool rhs);
    static bool And(bool lhs, bool rhs);
    static bool Not(bool hs);
    static bool overlap(Span lhs, Span rhs);
    static bool Longer(Span lhs, Span rhs);
    static bool Shorter(Span lhs, Span rhs);
    static bool Earlier(Span lhs, Span rhs);
    static bool Later(Span lhs, Span rhs);

private:
    Storage *store;
};


#endif //AAQL_AQL_FUNC_H
