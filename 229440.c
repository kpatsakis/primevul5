virSecuritySELinuxRestoreFileLabels(virSecurityManager *mgr,
                                    const char *path)
{
    int ret = 0;
    struct dirent *ent;
    char *filename = NULL;
    g_autoptr(DIR) dir = NULL;

    if ((ret = virSecuritySELinuxRestoreFileLabel(mgr, path, true)))
        return ret;

    if (!virFileIsDir(path))
        return 0;

    if (virDirOpen(&dir, path) < 0)
        return -1;

    while ((ret = virDirRead(dir, &ent, path)) > 0) {
        filename = g_strdup_printf("%s/%s", path, ent->d_name);
        ret = virSecuritySELinuxRestoreFileLabel(mgr, filename, true);
        VIR_FREE(filename);
        if (ret < 0)
            break;
    }
    if (ret < 0)
        virReportSystemError(errno, _("Unable to restore file labels under %s"),
                             path);

    return ret;
}