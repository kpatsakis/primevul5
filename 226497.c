lr_yum_repo_update(LrYumRepo *repo, const char *type, const char *path)
{
    assert(repo);
    assert(type);
    assert(path);

    for (GSList *elem = repo->paths; elem; elem = g_slist_next(elem)) {
        LrYumRepoPath *yumrepopath = elem->data;
        assert(yumrepopath);

        if (!strcmp(yumrepopath->type, type)) {
            lr_free(yumrepopath->path);
            yumrepopath->path = g_strdup(path);
            return;
        }
    }

    lr_yum_repo_append(repo, type, path);
}