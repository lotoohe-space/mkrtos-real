#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
extern void *p2c_addr(void *addr);
int main(int argv, char *args[])
{
        //在fork前创建管道
        int pipefd[2];//里边有0和1
        int ret=pipe(pipefd);
        if(ret==-1)
        {
                perror("pipe:");
                exit(1);
        }
        pid_t pid=fork();
        if(pid>0)
        {
			//父进程
			printf("i am parent process,pid:%d\n",getpid());

			char *strs="i am parent process!";
			char buf[64]={0};
			while(1)
			{
				int len=read(pipefd[0],buf,sizeof(buf));//读取来自子进程发来的数据
				printf("parent recv:%s,pid:%d\n",buf,getpid());
			   //向子进程发送数据
				write(pipefd[1],strs,strlen(strs));
				sleep(1);
			}
        }else if(pid==0)
        {
        	int *pipefd_c = p2c_addr(pipefd);
        	//子进程
			printf("i am child process,pid:%d\n",getpid());

			char * str="hello,i am child";
			char buff[64]={0};
			while(1)
			{
				 //向父进程发送数据
				write(pipefd_c[1],str,strlen(str));
				sleep(1);
				//读取父进程发送来的数据
				int fd=read(pipefd_c[0],buff,sizeof(buff));
				printf("child recv:%s,pid:%d\n",buff,getpid());
			}
        }
        return 0;
}
