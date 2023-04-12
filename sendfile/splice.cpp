#include "tcp_svr.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define CHECK_RET(p) if(p == false){return 0;}//判断封装接口是否调用成功，失败直接返回

int main(int argc, char* argv[])
{
	argv[1] = "192.168.152.128";
	argv[2] = "4444";
	Tcpsvr svr;
	CHECK_RET(svr.Createsocket());
	
	CHECK_RET(svr.Bind(argv[1], atoi(argv[2])));
	CHECK_RET(svr.Listen());

	Tcpsvr cln;
	struct sockaddr_in addr;
	svr.Accept(&cln, &addr);

	char buf1[1024] = {0};
	int len1 = svr.Recv(cln.GetFd(), buf1);
	if (len1 < 0)
	{
		printf("recv error\n");
	}
	buf1[len1] = '\0';
	printf("server read = %s\n", buf1);

	char *buf2 = "789";
    CHECK_RET(svr.Send(cln.GetFd(), buf2));
    
	svr.Close();
	cln.Close();
}
