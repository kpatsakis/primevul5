screendump_command()
{
    c_token++;
#ifdef _WIN32
    screen_dump();
#else
    fputs("screendump not implemented\n", stderr);
#endif
}