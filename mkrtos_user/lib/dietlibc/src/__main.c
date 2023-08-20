
int __app_start_addr__ = 0;

extern int main(int argc, char **argv);

void __main(int *args, int start_addr)
{
    int ret;

    __app_start_addr__ = start_addr;
    ret = main(args[0], (char **)&args[1]);

    exit(ret);
}
