static void store_proxy(const char *server, void *data __attribute__((unused)),
                        void *rock)
{
    struct proxy_rock *prock = (struct proxy_rock *) rock;

    proxy_store_func(server, prock->mbox_pat, prock->entryatts);
}