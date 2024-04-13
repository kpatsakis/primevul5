yum_repo_path(LrYumRepo *repo, const char *type)
{
    assert(repo);

    for (GSList *elem = repo->paths; elem; elem = g_slist_next(elem)) {
        LrYumRepoPath *yumrepopath = elem->data;
        assert(yumrepopath);
        if (!strcmp(yumrepopath->type, type))
            return yumrepopath->path;
    }
    return NULL;
}