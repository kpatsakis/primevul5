lr_yum_repo_free(LrYumRepo *repo)
{
    if (!repo)
        return;

    for (GSList *elem = repo->paths; elem; elem = g_slist_next(elem)) {
        LrYumRepoPath *yumrepopath = elem->data;
        assert(yumrepopath);
        lr_free(yumrepopath->type);
        lr_free(yumrepopath->path);
        lr_free(yumrepopath);
    }

    g_slist_free(repo->paths);
    lr_free(repo->repomd);
    lr_free(repo->url);
    lr_free(repo->destdir);
    lr_free(repo->signature);
    lr_free(repo->mirrorlist);
    lr_free(repo->metalink);
    lr_free(repo);
}