#include "aql_func.h"

bool AQLFunc::FollowTok(Span lhs, Span rhs, int min, int max) {
    return rhs.wbegin-lhs.wend>min && rhs.wbegin-lhs.wend<=max+1;
}

Span AQLFunc::CombineSpans(Span lhs, Span rhs) {
    if (lhs.begin < 0)
        return rhs;

    if (rhs.begin < 0)
        return lhs;

    Span res;
    res.begin = min(lhs.begin, rhs.begin);
    res.end = max(lhs.end, rhs.end);
    res.wbegin = min(lhs.wbegin, rhs.wbegin);
    res.wend = max(lhs.wend, rhs.wend);
    return res;
}

bool AQLFunc::Or(bool lhs, bool rhs) {
    return lhs || rhs;
}

bool AQLFunc::And(bool lhs, bool rhs) {
    return lhs && rhs;
}

bool AQLFunc::Not(bool hs) {
    return !hs;
}

bool AQLFunc::overlap(Span lhs, Span rhs) {
    if (lhs.end < rhs.begin || rhs.end < lhs.begin) {
        return false;
    }
    return true;
}

bool AQLFunc::Longer(Span lhs, Span rhs) {
    return  (lhs.end - lhs.begin) > (rhs.end - rhs.begin) ? true : false;
}

bool AQLFunc::Shorter(Span lhs, Span rhs) {
    return (lhs.end - lhs.begin) < (rhs.end - rhs.begin) ? true : false;
}

bool AQLFunc::Earlier(Span lhs, Span rhs) {
    return lhs.begin < rhs.begin ? true : false;
}

bool AQLFunc::Later(Span lhs, Span rhs) {
    return lhs.begin > rhs.begin ? true : false;
}

bool AQLFunc::True() {
    return true;
}
