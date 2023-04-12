#include "tcp_svr.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#define CHECK_RET(p) if(p == false){return 0;}//判断封装接口是否调用成功，失败直接返回
int main(int argc, char* argv[])
{
    Tcpsvr tp;
    //1、创建套接字
    CHECK_RET(tp.Createsocket());
    //2、绑定地址信息
    printf("bind\n");
    CHECK_RET(tp.Bind(argv[1], argv[2]));
    //3、监听
    printf("listen\n");
    CHECK_RET(tp.Listen());
    Tcpsvr newts;
    while(1)
    {
        //4、获取连接
        struct sockaddr_in cli_addr;
        printf("accept\n");
        CHECK_RET(tp.Accept(&newts,&cli_addr));
        //5、打开文件
        printf("open\n");
        int file_fd = open("./wyg.txt",O_RDWR | O_CREAT,664);
        if(file_fd < 0)
        {
            perror("open");
            return -1;
        }

        FILE* fid = fopen("./wyg.txt", "w");
        if (fid == NULL)
        {
            printf("打开文件失败\n");
            return 0;
        }
        fprintf(fid, "test");
        fclose(fid);
        
		//6、//设置file_fd文件描述符属性
        struct stat stat_buf;
        fstat(file_fd,&stat_buf);
		//7、//把目标文件传递给服务器
        int ret = sendfile(newts.GetFd(),file_fd,NULL,stat_buf.st_size);
        if(ret < 0)
        {
            perror("sendfile");
            return -1;
        }
    }
    //8、关闭套接字
    tp.Close();
    newts.Close();
    return 0;
}
