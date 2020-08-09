#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#define SERV_PORT 9877
#define MAXLINE 1204

using namespace std;

void str_cli(FILE *fp,int sockfd)
{
    fd_set set;
    FD_ZERO(&set);

    char buff[1024];
    int stdineof=0,n;
    while(1)
    {
        if(stdineof==0)
            FD_SET(fileno(fp),&set);
        FD_SET(sockfd,&set);

        int maxfd=max(fileno(fp),sockfd)+1;
        
        select(maxfd,&set,NULL,NULL,NULL);

        if(FD_ISSET(sockfd,&set))
        {
            if((n=read(sockfd,buff,MAXLINE))==0)
                if(stdineof==1)
                    return;
                else
                    cerr<<"str_cli: server terinated peraturely"<<endl;

            write(fileno(stdout),buff,n);
        }
        else if(FD_ISSET(fileno(fp),&set))
        {
            if((n=read(fileno(fp),buff,MAXLINE))==0)//客户完成输入
                stdineof=1;
            write(sockfd,buff,n);

            //shutdown(sockfd,SHUT_WR);

            FD_CLR(fileno(fp),&set);
            continue;
        }
        //write(sockfd,buff,n);
    }
    return;
}

int main(int argc,char **argv)
{
    if(argc!=2)
    {
        cerr<<"please input server address."<<endl;
        exit(1);
    }

    int sockfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    servaddr.sin_port=htons(SERV_PORT);
    
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    str_cli(stdin,sockfd);
    return 0;
}
