lr_yum_repo_path(LrYumRepo *repo, const char *type)
{
    assert(repo);

    gchar *chk_type = get_type(repo, type);
    const char *path = yum_repo_path(repo, chk_type);
    g_free(chk_type);
    return path;
}