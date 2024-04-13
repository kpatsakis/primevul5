static void dec2str(int val, char *str, size_t size)
{
    if (val == -1)
        snprintf(str, size, "*");
    else
        snprintf(str, size, "%d", val); 
}