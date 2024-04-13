lr_yum_repo_append(LrYumRepo *repo, const char *type, const char *path)
{
    assert(repo);
    assert(type);
    assert(path);

    LrYumRepoPath *yumrepopath = lr_malloc(sizeof(LrYumRepoPath));
    yumrepopath->type = g_strdup(type);
    yumrepopath->path = g_strdup(path);
    repo->paths = g_slist_append(repo->paths, yumrepopath);
}