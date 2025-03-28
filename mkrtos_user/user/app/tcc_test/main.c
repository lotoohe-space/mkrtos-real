

int a;
int b;
int _start_c(void)
{
    a = 1;
    b = 2;
    while(a++ < 100)
    {
        u_log_write_bytes(4, "tcc test\n", 9);
    }
   
    return a+b;
}