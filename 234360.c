do_string(const char *s)
{
    char *cmdline = gp_strdup(s);
    do_string_and_free(cmdline);
}