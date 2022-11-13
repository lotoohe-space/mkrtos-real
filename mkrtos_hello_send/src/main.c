
//#include <stdio.h>

extern int fcall(int call_no, int arg0,int arg1, int arg2);
extern void fcall_exit(void);
extern int slot_reg(char* func_name, void *fc, void *exit_fc);

int main(int argc, char **argv)
{
	int i=0;
	int j=0;
	int m=0;

	//printf("Hello world.\n");
	while(1){
		sleep(1);
		fcall(0, i++,j++,m++);
	}
	write(1,"en\n",4);
	return 0;
}
