void pidfile_remove(void)
{
    if (!g_pidfile)
        return;

    unlink(g_pidfile);

    free(g_pidfile);
    g_pidfile = NULL;
}