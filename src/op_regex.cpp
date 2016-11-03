#include "op_regex.h"
#include "util.h"

View OpRegex::eval() {
    View vi;
    Span span;
    wcmatch match;
    wregex pattern(MBs2WCs(reg_expr.c_str()));
    const wchar_t *start, *low, *high;
    string group_name[pattern.mark_count()+1];

    // create span columns for regex matching results
    for (int i=0; i<=pattern.mark_count(); i++) {
        group_name[i] = Operator::get_group_name(i);
        vi.cols[group_name[i]] = new SpanColumn(group_name[i]);
    }

    low = start = store->text.c_str();
    high = start+store->text.length();
    while (regex_search(low, high, match, pattern)) {
        if (match.length(0)) {
            for (int i = 0; i < match.size(); i++) {
                span.begin = low - start + match.position(i);
                span.end = span.begin + match.length(i);
                span.wbegin = store->c2w(span.begin);
                span.wend = store->c2w(span.end - 1);
                ((SpanColumn *)vi.cols[group_name[i]])->spans.push_back(span);
            }
            low += match.position(0)+match.length(0);
        } else {
            // only match empty string, move on to avoid dead loop
            if (high-low > 1)
                low++;
            else
                break;
        }
    }

    return vi;
}

void OpRegex::dump(int spaces, int tab) {
    fprintf(stdout, "%*s+Regex\n", spaces, "");
    fprintf(stdout, "%*s+RegExpr\n", spaces+tab, "");
    fprintf(stdout, "%*s%s\n", spaces+2*tab, "", reg_expr.c_str());
}
