#include <windows.h>
#include <stdio.h>
#include "test.h"

static BYTE *ddeString = NULL;
static WCHAR *buf = NULL;
static HSZ hszTopic = 0, hszReturn = 0;
static DWORD ddeInst = 0;

static HDDEDATA CALLBACK ddeCb(UINT uType, UINT uFmt, HCONV hConv,
                                HSZ hsz1, HSZ hsz2, HDDEDATA hData,
                                ULONG_PTR dwData1, ULONG_PTR dwData2)
{
    DWORD size = 0, ret = 0;

    switch (uType)
    {
        case XTYP_CONNECT:
            if (!DdeCmpStringHandles(hsz1, hszTopic))
                return (HDDEDATA)TRUE;
            return (HDDEDATA)FALSE;

        case XTYP_EXECUTE:
            if (!(size = DdeGetData(hData, NULL, 0, 0)))
                printf("DdeGetData returned zero size of execute string\n");
            else if (!(ddeString = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)))
                printf("Out of memory\n");
            else if (DdeGetData(hData, ddeString, size, 0) != size)
                printf("DdeGetData did not return %ld bytes\n", size);
            DdeFreeDataHandle(hData);
            return (HDDEDATA)DDE_FACK;

        case XTYP_REQUEST:
            ret = -3; /* error */
            if (!(size = DdeQueryStringW(ddeInst, hsz2, NULL, 0, CP_WINUNICODE)))
                printf("DdeQueryString returned zero size of request string\n");
            else if (!(buf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size + 1) * sizeof(WCHAR))))
                printf("Out of memory\n");
            else if (DdeQueryStringW(ddeInst, hsz2, buf, size + 1, CP_WINUNICODE) != size)
                printf("DdeQueryString did not return %ld characters\n", size);
            else
                ret = -2; /* acknowledgment */
            return DdeCreateDataHandle(ddeInst, (LPBYTE)&ret, sizeof(ret), 0,
                                       hszReturn, CF_TEXT, 0);

        default:
            return NULL;
    }
}

int main()
{
    static const CHAR szApplication[] = "IExplore";
    static const CHAR szTopic[] = "WWW_OpenURL";
    static const CHAR szReturn[] = "Return";
    static const WCHAR szApplicationW[] = {'I','E','x','p','l','o','r','e',0};
    static const WCHAR szTopicW[] = {'W','W','W','_','O','p','e','n','U','R','L',0};
    static const WCHAR szReturnW[] = {'R','e','t','u','r','n',0};
    BOOL type_a = FALSE;
    HSZ hszApplication = 0;
    UINT_PTR timer = 0;
    int i;

    printf("dde_server - startup\n");
    if (type_a)
    {
        DdeInitializeA(&ddeInst, ddeCb, CBF_SKIP_ALLNOTIFICATIONS | CBF_FAIL_ADVISES | CBF_FAIL_POKES, 0);
        hszApplication = DdeCreateStringHandleA(ddeInst, szApplication, CP_WINANSI);
        hszTopic = DdeCreateStringHandleA(ddeInst, szTopic, CP_WINANSI);
        hszReturn = DdeCreateStringHandleA(ddeInst, szReturn, CP_WINANSI);
    }
    else
    {
        DdeInitializeW(&ddeInst, ddeCb, CBF_SKIP_ALLNOTIFICATIONS | CBF_FAIL_ADVISES | CBF_FAIL_POKES, 0);
        hszApplication = DdeCreateStringHandleW(ddeInst, szApplicationW, CP_WINUNICODE);
        hszTopic = DdeCreateStringHandleW(ddeInst, szTopicW, CP_WINUNICODE);
        hszReturn = DdeCreateStringHandleW(ddeInst, szReturnW, CP_WINUNICODE);
    }
    DdeNameService(ddeInst, hszApplication, 0, DNS_REGISTER);
    timer = SetTimer(NULL, 0, 5000, NULL);
    while (!ddeString)
    {
        MSG msg;
        if (!GetMessageW(&msg, NULL, 0, 0)) break;
        if (msg.message == WM_TIMER) break;
        DispatchMessageW(&msg);
    }

    if (timer) KillTimer(NULL, timer);
    if (ddeInst)
    {
        if (hszTopic && hszApplication) DdeNameService(ddeInst, hszApplication, 0, DNS_UNREGISTER);
        if (hszReturn) DdeFreeStringHandle(ddeInst, hszReturn);
        if (hszTopic) DdeFreeStringHandle(ddeInst, hszTopic);
        if (hszApplication) DdeFreeStringHandle(ddeInst, hszApplication);
        DdeUninitialize(ddeInst);
    }
    printf("--WCHAR--\n");
    printf("%s\n", wine_dbgstr_w((WCHAR *)ddeString));
    for(i = 0; i < lstrlenW((WCHAR *)ddeString); i++)
    {
        printf("%x\n", ((WCHAR *)ddeString)[i]);
    }
    printf("--CHAR--\n");
    printf("%s\n", (CHAR *)ddeString);
    for(i = 0; i < lstrlenA((CHAR *)ddeString); i++)
    {
        printf("%x\n", ((CHAR *)ddeString)[i]);
    }

    return 0;
}
