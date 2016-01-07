
#include "stdafx.h"
#include <winsock2.h>
#include <Iphlpapi.h>





//      ��һ��tcp�����˿�
int os_tcp_listen(const char *ipaddr,int port)
{
    struct  sockaddr_in     addr;
    int                     sd;

    sd  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if( sd == -1 )
    {
        printf("ERROR:  create svr socket fail\n");
        return -1;
    }

    memset(&addr,0,sizeof(addr));

    addr.sin_family     = AF_INET;
    addr.sin_port       = htons(port);

    if( ipaddr )
        addr.sin_addr.s_addr    = inet_addr(ipaddr);

    if ( bind (sd,(struct sockaddr *)&addr,sizeof(sockaddr_in) ) != 0 )
    {
        printf("ERROR:  bind socket fail\n");
        closesocket(sd);
        return -1;
    }

    if( listen (sd,100) != 0 )
    {
        printf("ERROR:  listen socket fail\n");
        closesocket(sd);
        return -1;
    }

    return sd;
}

//	���ӵ�ָ��IP��ַ
int os_tcp_connect(const char *ipaddr,int port)
{
    struct  sockaddr_in     addr;
    int                     sd;

    sd  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if( sd == -1 )
    {
        printf("ERROR:  create svr socket fail\n");
        return -1;
    }

    memset(&addr,0,sizeof(addr));

    addr.sin_family     = AF_INET;
    addr.sin_port       = htons(port);

    if( ipaddr )
        addr.sin_addr.s_addr    = inet_addr(ipaddr);

    if ( connect (sd,(struct sockaddr *)&addr,sizeof(sockaddr_in) ) != 0 )
    {
        printf("ERROR:  bind socket fail\n");
        closesocket(sd);
        return -1;
    }

    return sd;
}

int os_tcp_connect_to(const char *ipaddr,int port,int conn_to)
{
    struct sockaddr_in server;
    int sd  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if( sd == -1 ){
        printf("ERROR:  create svr socket fail\n");
        return -1;
    }

    int TimeOut = 6000;
    if(::setsockopt(sd,SOL_SOCKET,SO_SNDTIMEO,(char *)&TimeOut,sizeof(TimeOut))==SOCKET_ERROR){
        return 0;
    }
    TimeOut=6000;//���ý��ճ�ʱ6��
    if(::setsockopt(sd,SOL_SOCKET,SO_RCVTIMEO,(char *)&TimeOut,sizeof(TimeOut))==SOCKET_ERROR){
        return 0;
    }

    //���÷�������ʽ����
    unsigned long ul = 1;
    int ret = ioctlsocket(sd, FIONBIO, (unsigned long*)&ul);
    if(ret==SOCKET_ERROR)return 0;

    //����
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr .s_addr = inet_addr((LPCSTR)ipaddr);
    if(server.sin_addr.s_addr == INADDR_NONE){return 0;}

    connect(sd,(const struct sockaddr *)&server,sizeof(server));

    //select ģ�ͣ������ó�ʱ
    struct timeval timeout ;
    fd_set r;

    FD_ZERO(&r);
    FD_SET(sd, &r);
    timeout.tv_sec = conn_to; //���ӳ�ʱ15��
    timeout.tv_usec =0;
    ret = select(0, 0, &r, 0, &timeout);
    if ( ret <= 0 )
    {
        ::closesocket(sd);
        return 0;
    }
    //һ�������ģʽ�׽ӱȽ��ѿ��ƣ����Ը���ʵ��������� ���������ģʽ
    unsigned long ul1= 0 ;
    ret = ioctlsocket(sd, FIONBIO, (unsigned long*)&ul1);
    if(ret==SOCKET_ERROR){
        ::closesocket (sd);
        return 0;
    }

    return sd;
}

int os_tcp_close(int sd)
{
    return closesocket(sd);
}

int os_writen(int sd,char *buf,int size)
{
    while( size > 0 )
    {
        int ret = send(sd,buf,size,0);
        if( ret <= 0 )
        {
            return -1;
        }

        buf     += ret;
        size    -= ret;
    }

    return 0;
}


int os_startthr( LPTHREAD_START_ROUTINE func,void *param)
{
    /* ���������߳� */
    HANDLE  handle = ::CreateThread(NULL, 0, func , param, 0, NULL);
    if( handle == NULL ) {
        return -1;
    }
    else {
        /* �ͷ��߳���Դ���൱��linux��detach */
        CloseHandle(handle);
    }

    return 0;
}

/* ���ر���IP��ַ */
int os_pcapsock_getselfip(char *strIP)
{
    //��¼��������
    int         netCardNum          = 0;
    //��¼ÿ�������ϵ�IP��ַ����
    int         IPnumPerNetCard     = 0;
    int         find		    = 0;


    //PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
    //�õ��ṹ���С,����GetAdaptersInfo����
    //����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
    IP_ADAPTER_INFO                 tTmpAdapter;
    PIP_ADAPTER_INFO                pIpAdapterInfo = &tTmpAdapter;
    unsigned long       stSize          = sizeof(IP_ADAPTER_INFO);

    int                 nRel            = GetAdaptersInfo(pIpAdapterInfo,&stSize);


    if (ERROR_BUFFER_OVERFLOW == nRel)
    {
        //����������ص���ERROR_BUFFER_OVERFLOW
        //��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
        //��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
        //�ͷ�ԭ�����ڴ�ռ�
        //���������ڴ�ռ������洢����������Ϣ
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
        if( ! pIpAdapterInfo )
            return -1;

        //�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
        nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
    }

    if (ERROR_SUCCESS != nRel)
    {
        if( pIpAdapterInfo != &tTmpAdapter )
            delete[] pIpAdapterInfo;
        return -1;
    }

    //���������Ϣ
    //�����ж�����,���ͨ��ѭ��ȥ�ж�
    while (pIpAdapterInfo && find == 0)
    {
        //  ֻ��¼��̫������Ϣ
        if( pIpAdapterInfo->Type != MIB_IF_TYPE_ETHERNET )
        {
            pIpAdapterInfo = pIpAdapterInfo->Next;
            continue;
        }

        IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);

        //  ֻ��¼��һ�� IP ��ַ
        while( pIpAddrString ){
            find        = 1;
            strcpy(strIP,pIpAddrString->IpAddress.String);
            break;
        }
    }

    //�ͷ��ڴ�ռ�
    if( pIpAdapterInfo != &tTmpAdapter )
        delete[] pIpAdapterInfo;

    if( !find ){
        return -1;
    }
    return 0;
}


