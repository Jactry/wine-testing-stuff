#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static HDDEDATA CALLBACK dde_cb(UINT uType, UINT uFmt, HCONV hConv,
                                HSZ hsz1, HSZ hsz2, HDDEDATA hData,
                                ULONG_PTR dwData1, ULONG_PTR dwData2)
{
    return NULL;
}

int main()
{
    BOOL type_a = TRUE;
    DWORD ddeInst = 0;
    HSZ hszApp, hszTopic;
    HCONV hConv;
    char app[] = "IExplore";
    char topic[] = "WWW_OpenURL";
    WCHAR appW[] = {'I','E','x','p','l','o','r','e',0};
    WCHAR topicW[] = {'W','W','W','_','O','p','e','n','U','R','L',0};
    WCHAR dde_server[] = {'d','d','e','_','s','e','r','v','e','r','.','e','x','e',0};
    WCHAR data[] = {0x0061, 0x0062, 0x0063, 0x9152, 0};
    STARTUPINFOW startup;
    PROCESS_INFORMATION proc;
    HDDEDATA hDdeData;
    DWORD tid;

    if (type_a)
    {
        DdeInitializeA(&ddeInst, dde_cb, APPCMD_CLIENTONLY, 0L);
        hszApp = DdeCreateStringHandleA(ddeInst, app, CP_WINANSI);
        hszTopic = DdeCreateStringHandleA(ddeInst, topic, CP_WINANSI);
    }
    else
    {
        DdeInitializeW(&ddeInst, dde_cb, APPCMD_CLIENTONLY, 0L);
        hszApp = DdeCreateStringHandleW(ddeInst, appW, CP_WINUNICODE);
        hszTopic = DdeCreateStringHandleW(ddeInst, topicW, CP_WINUNICODE);
    }
    hConv = DdeConnect(ddeInst, hszApp, hszTopic, NULL);
    if(!hConv)
    {
        printf("Success\n");
        ZeroMemory(&startup, sizeof(STARTUPINFOW));
        startup.cb = sizeof(startup);
        startup.dwFlags = STARTF_USESHOWWINDOW;
        startup.wShowWindow = SW_SHOWNORMAL;
        printf("CreateProcess - %d\n", CreateProcessW(NULL, dde_server, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &startup, &proc));
        printf("CreateProcess - GetLastError: %lx\n", GetLastError());
        Sleep(1000); /* wait winebrowser startup */
        hConv = DdeConnect(ddeInst, hszApp, hszTopic, NULL);
        if(!hConv)
            printf("Should fail\n");
    }
    hDdeData = DdeClientTransaction((LPBYTE)data, (lstrlenW(data) + 1) * sizeof(WCHAR), hConv, 0L, 0,
                                    XTYP_EXECUTE, 30000, &tid);
    if (hDdeData)
    {
        DdeFreeDataHandle(hDdeData);
        printf("DdeClientTransaction success\n");
    }
    else
        printf("DdeClientTransaction failed: %x\n", DdeGetLastError(ddeInst));

    return 0;
}
