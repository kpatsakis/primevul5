virSecuritySELinuxLXCInitialize(virSecurityManager *mgr)
{
    g_autoptr(virConf) selinux_conf = NULL;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    data->skipAllLabel = true;

    data->label_handle = selabel_open(SELABEL_CTX_FILE, NULL, 0);
    if (!data->label_handle) {
        virReportSystemError(errno, "%s",
                             _("cannot open SELinux label_handle"));
        return -1;
    }

    if (!(selinux_conf = virConfReadFile(selinux_lxc_contexts_path(), 0)))
        goto error;

    if (virConfGetValueString(selinux_conf, "process", &data->domain_context) < 0)
        goto error;

    if (!data->domain_context) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("missing 'process' value in selinux lxc contexts file '%s'"),
                       selinux_lxc_contexts_path());
        goto error;
    }

    if (virConfGetValueString(selinux_conf, "file", &data->file_context) < 0)
        goto error;

    if (!data->file_context) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("missing 'file' value in selinux lxc contexts file '%s'"),
                       selinux_lxc_contexts_path());
        goto error;
    }

    if (virConfGetValueString(selinux_conf, "content", &data->content_context) < 0)
        goto error;

    if (!data->content_context) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("missing 'content' value in selinux lxc contexts file '%s'"),
                       selinux_lxc_contexts_path());
        goto error;
    }

    if (!(data->mcs = virHashNew(NULL)))
        goto error;

    return 0;

 error:
    selabel_close(data->label_handle);
    data->label_handle = NULL;
    VIR_FREE(data->domain_context);
    VIR_FREE(data->file_context);
    VIR_FREE(data->content_context);
    virHashFree(data->mcs);
    return -1;
}