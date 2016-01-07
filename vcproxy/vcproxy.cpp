// tcpproxy.cpp : �������̨Ӧ�ó������ڵ㡣
//				ʵ��ԭ��	
//

#include "stdafx.h"
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��





static void help(const char *appname){
    printf("help: %s [-l localip ] [-p proxyip] [-s ipaddr/mask[:port=22] ]\n",appname);
}

int main(int argc, _TCHAR* argv[])
{
    WSADATA wsaData;
    int Ret;
    struct proxyargs param;

    if( init_args(&param,argc,argv) != 0 ){
        help(argv[0]);
        exit(1);
    }


    // Initialize Winsock version 2.2
    if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
    {
        // NOTE: Since Winsock failed to load we cannot use
        // WSAGetLastError to determine the specific error for
        // why it failed. Instead we can rely on the return
        // status of WSAStartup.
        printf("WSAStartup failed with error %d\n", Ret);
        return 0;
    }

    if(param.scan_ip != 0 ) {
        printf("=====   TCP SCAN MODE   =====\n");
        run_scanip(&param);
    }
    else {
        printf("=====   TCP PROXY MODE  =====\n");
        printf("Local IP:%s remote IP:%s\n",param.localip,param.proxyip);
        run_tcpproxy(param.localip,param.proxyip);
    }

    // Setup Winsock communication code here
    // When your application is finished call WSACleanup
    if (WSACleanup() == SOCKET_ERROR)
    {
        printf("WSACleanup failed with error %d\n", WSAGetLastError());
    }

    return 0;
}














