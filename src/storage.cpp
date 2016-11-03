#include "storage.h"

Storage::Storage() {

}

Storage::~Storage() {
    for (auto x : dicts)
        delete(x.second);
}

void Storage::clear() {
    text = L"";
    words.clear();
    for (auto x : views)
        x.second.clear();
    views.clear();
}

int Storage::c2w(int ci) {
    if (ci<0 || ci>=text.size())
        return -1;
    return bsearch(0, words.size()-1, ci);
}

int Storage::bsearch(int low, int high, int ci) {
    if (low == high) {
        // in case of whitespace, return its next word's index(except the last word)
        if (ci<words[low].end || low==words.size()-1)
            return low;
        else
            return low+1;
    }

    int middle = low+(high-low)/2;
    if (ci < words[middle].begin)
        return bsearch(low, middle-1, ci);
    else if (ci >= words[middle].end)
        return bsearch(middle+1, high, ci);
    else
        return middle;
}

void Storage::add_view(View vi) {
    views[vi.name] = vi;
}

View Storage::get_view(string view_name) {
    return views[view_name];
}
