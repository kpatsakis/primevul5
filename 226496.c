get_type(LrYumRepo *repo, const char *type)
{
    if (!repo->use_zchunk)
        return g_strdup(type);

    gchar *chk_type = g_strconcat(type, "_zck", NULL);

    for (GSList *elem = repo->paths; elem; elem = g_slist_next(elem)) {
        LrYumRepoPath *yumrepopath = elem->data;
        assert(yumrepopath);
        if (!strcmp(yumrepopath->type, chk_type))
            return chk_type;
    }
    g_free(chk_type);
    return g_strdup(type);
}