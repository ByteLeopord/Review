#include "epollserver.h"
#define INDARRY_PORT 9877

using namespace std;

int main(int argc,char **argv)
{
    Epollserver *epoll=new Epollserver(INDARRY_PORT,20);
    epoll->init();//对初始化服务器(socket，bind，listen,epoll_create...等函数，病完成线程池的初始化)
    epoll->epoll();//开启线程池，完成相应的任务添加之后，自动调用线程池中空闲的函数来完成doit工作
    return 0;
}
