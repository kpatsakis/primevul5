virSecuritySELinuxGenNewContext(const char *basecontext,
                                const char *mcs,
                                bool isObjectContext)
{
    context_t context = NULL;
    char *ret = NULL;
    char *str;
    char *ourSecContext = NULL;
    context_t ourContext = NULL;

    VIR_DEBUG("basecontext=%s mcs=%s isObjectContext=%d",
              basecontext, mcs, isObjectContext);

    if (getcon_raw(&ourSecContext) < 0) {
        virReportSystemError(errno, "%s",
                             _("Unable to get current process SELinux context"));
        goto cleanup;
    }
    if (!(ourContext = context_new(ourSecContext))) {
        virReportSystemError(errno,
                             _("Unable to parse current SELinux context '%s'"),
                             ourSecContext);
        goto cleanup;
    }
    VIR_DEBUG("process=%s", ourSecContext);

    if (!(context = context_new(basecontext))) {
        virReportSystemError(errno,
                             _("Unable to parse base SELinux context '%s'"),
                             basecontext);
        goto cleanup;
    }

    if (context_user_set(context,
                         context_user_get(ourContext)) != 0) {
        virReportSystemError(errno,
                             _("Unable to set SELinux context user '%s'"),
                             context_user_get(ourContext));
        goto cleanup;
    }

    if (!isObjectContext &&
        context_role_set(context,
                         context_role_get(ourContext)) != 0) {
        virReportSystemError(errno,
                             _("Unable to set SELinux context role '%s'"),
                             context_role_get(ourContext));
        goto cleanup;
    }

    if (context_range_set(context, mcs) != 0) {
        virReportSystemError(errno,
                             _("Unable to set SELinux context MCS '%s'"),
                             mcs);
        goto cleanup;
    }
    if (!(str = context_str(context))) {
        virReportSystemError(errno, "%s",
                             _("Unable to format SELinux context"));
        goto cleanup;
    }
    ret = g_strdup(str);
    VIR_DEBUG("Generated context '%s'",  ret);
 cleanup:
    freecon(ourSecContext);
    context_free(ourContext);
    context_free(context);
    return ret;
}