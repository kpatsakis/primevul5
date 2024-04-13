static bool sst_receive_address_check (const char* str)
{
    if (!strncasecmp(str, "127.0.0.1", strlen("127.0.0.1")) ||
        !strncasecmp(str, "localhost", strlen("localhost")))
    {
        return 1;
    }

    return 0;
}