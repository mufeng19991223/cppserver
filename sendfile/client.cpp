#include "tcp_svr.h"
#define CHECK_RET(p) if(p == false){return 0;}//判断封装的接口是否调用成功，失败直接返回
int main(int argc, char* argv[])
{
    Tcpsvr tp;
    CHECK_RET(tp.Createsocket());

    CHECK_RET(tp.Connect(argv[1], atoi(argv[2])));

    char *buf1 = "996644";
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
