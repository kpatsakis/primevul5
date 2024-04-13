virSecuritySELinuxSetFileLabels(virSecurityManager *mgr,
                                const char *path,
                                virSecurityLabelDef *seclabel)
{
    int ret = 0;
    struct dirent *ent;
    char *filename = NULL;
    g_autoptr(DIR) dir = NULL;

    if ((ret = virSecuritySELinuxSetFilecon(mgr, path, seclabel->imagelabel, true)))
        return ret;

    if (!virFileIsDir(path))
        return 0;

    if (virDirOpen(&dir, path) < 0)
        return -1;

    while ((ret = virDirRead(dir, &ent, path)) > 0) {
        filename = g_strdup_printf("%s/%s", path, ent->d_name);
        ret = virSecuritySELinuxSetFilecon(mgr, filename,
                                           seclabel->imagelabel, true);
        VIR_FREE(filename);
        if (ret < 0)
            break;
    }
    if (ret < 0)
        virReportSystemError(errno, _("Unable to label files under %s"),
                             path);

    return ret;
}