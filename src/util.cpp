#include "util.h"

wstring MBs2WCs(const char* pszSrc) {
    wchar_t* pwcs = NULL;
    wstring wres;
    int size = 0;

    size = mbstowcs(NULL, pszSrc, 0);
    pwcs = (wchar_t *)malloc((size+1)*sizeof(wchar_t));
    size = mbstowcs(pwcs, pszSrc, size+1);
    pwcs[size] = 0;
    wres = pwcs;
    free(pwcs);

    return wres;
}

string WCs2MBs(const wchar_t *wcharStr) {
    char *str = NULL;
    string res;
    int size = 0;

    size = wcstombs(NULL, wcharStr, 0);
    str = (char *)malloc((size+1)*sizeof(char));
    wcstombs(str, wcharStr, size);
    str[size] = '\0';
    res = str;
    free(str);

    return res;
}