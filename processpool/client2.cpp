#include "tcp_svr.h"
#define CHECK_RET(p) if(p == false){return 0;}//判断封装的接口是否调用成功，失败直接返回
int main(int argc, char* argv[])
{
    Tcpsvr tp;
    const char* ip = "192.168.152.128";
    int port = 6666;
    CHECK_RET(tp.Createsocket());

    CHECK_RET(tp.Connect(ip, port));

    char buf1[1024] = "GET http://localhost/123.com HTTP/1.1\r\nHost:hello\r\n786424549\r\nqqq";
    CHECK_RET(tp.Send(tp.GetFd(), buf1));

    char buf2[1024] = { 0 };

    int len = tp.Recv(tp.GetFd(), buf2);
    if (len < 0)
    {
        printf("recv error\n");
    }
    
    buf2[len] = '\0';
    printf("client read = %s\n", buf2);

    tp.Close();
    return 0;
}
