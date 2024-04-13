virSecuritySELinuxQEMUInitialize(virSecurityManager *mgr)
{
    char *ptr;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    data->skipAllLabel = false;

    data->label_handle = selabel_open(SELABEL_CTX_FILE, NULL, 0);
    if (!data->label_handle) {
        virReportSystemError(errno, "%s",
                             _("cannot open SELinux label_handle"));
        return -1;
    }

    if (virFileReadAll(selinux_virtual_domain_context_path(), MAX_CONTEXT, &(data->domain_context)) < 0) {
        virReportSystemError(errno,
                             _("cannot read SELinux virtual domain context file '%s'"),
                             selinux_virtual_domain_context_path());
        goto error;
    }

    ptr = strchr(data->domain_context, '\n');
    if (ptr) {
        *ptr = '\0';
        ptr++;
        if (*ptr != '\0') {
            data->alt_domain_context = g_strdup(ptr);
            ptr = strchr(data->alt_domain_context, '\n');
            if (ptr)
                *ptr = '\0';
        }
    }
    VIR_DEBUG("Loaded domain context '%s', alt domain context '%s'",
              data->domain_context, NULLSTR(data->alt_domain_context));


    if (virFileReadAll(selinux_virtual_image_context_path(), 2*MAX_CONTEXT, &(data->file_context)) < 0) {
        virReportSystemError(errno,
                             _("cannot read SELinux virtual image context file %s"),
                             selinux_virtual_image_context_path());
        goto error;
    }

    ptr = strchr(data->file_context, '\n');
    if (ptr) {
        *ptr = '\0';
        data->content_context = g_strdup(ptr + 1);
        ptr = strchr(data->content_context, '\n');
        if (ptr)
            *ptr = '\0';
    }

    VIR_DEBUG("Loaded file context '%s', content context '%s'",
              data->file_context, data->content_context);

    if (!(data->mcs = virHashNew(NULL)))
        goto error;

    return 0;

 error:
    selabel_close(data->label_handle);
    data->label_handle = NULL;
    VIR_FREE(data->domain_context);
    VIR_FREE(data->alt_domain_context);
    VIR_FREE(data->file_context);
    VIR_FREE(data->content_context);
    virHashFree(data->mcs);
    return -1;
}