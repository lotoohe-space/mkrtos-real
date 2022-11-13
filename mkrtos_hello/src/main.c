
//#include <stdio.h>

extern int fcall(int call_no, int arg0,int arg1, int arg2);
extern void fcall_exit(void);
extern int slot_reg(char* func_name, void *fc, void *exit_fc);
int i=0;
void f_test(int arg0, int arg1, int arg2)
{
	char w= i%10 + 48;
	i++;
	//printf("ft:%d %d %d\n",arg0,arg1,arg2);
	write(1, &w, 1);
}

int main(int argc, char **argv)
{
	int i=0;
	int j=0;
	int m=0;

	//printf("fipc recevier.\n");
	slot_reg("f_test", f_test, fcall_exit);
//	write(1,"cl\n",4);
//	//printf("Hello world.\n");
//	while(1){
//		fcall(0, i++,j++,m++);
//	}
//	write(1,"en\n",4);
	while(1){
		;
	}
	return 0;
}
