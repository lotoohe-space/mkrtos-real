#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
extern void *p2c_addr(void *addr);
int main(int argv, char *args[])
{
        //��forkǰ�����ܵ�
        int pipefd[2];//�����0��1
        int ret=pipe(pipefd);
        if(ret==-1)
        {
                perror("pipe:");
                exit(1);
        }
        pid_t pid=fork();
        if(pid>0)
        {
			//������
			printf("i am parent process,pid:%d\n",getpid());

			char *strs="i am parent process!";
			char buf[64]={0};
			while(1)
			{
				int len=read(pipefd[0],buf,sizeof(buf));//��ȡ�����ӽ��̷���������
				printf("parent recv:%s,pid:%d\n",buf,getpid());
			   //���ӽ��̷�������
				write(pipefd[1],strs,strlen(strs));
				sleep(1);
			}
        }else if(pid==0)
        {
        	int *pipefd_c = p2c_addr(pipefd);
        	//�ӽ���
			printf("i am child process,pid:%d\n",getpid());

			char * str="hello,i am child";
			char buff[64]={0};
			while(1)
			{
				 //�򸸽��̷�������
				write(pipefd_c[1],str,strlen(str));
				sleep(1);
				//��ȡ�����̷�����������
				int fd=read(pipefd_c[0],buff,sizeof(buff));
				printf("child recv:%s,pid:%d\n",buff,getpid());
			}
        }
        return 0;
}
