virSecuritySELinuxContextAddRange(char *src,
                                  char *dst)
{
    char *str = NULL;
    char *ret = NULL;
    context_t srccon = NULL;
    context_t dstcon = NULL;

    if (!src || !dst)
        return ret;

    if (!(srccon = context_new(src)) || !(dstcon = context_new(dst))) {
        virReportSystemError(errno, "%s",
                             _("unable to allocate security context"));
        goto cleanup;
    }

    if (context_range_set(dstcon, context_range_get(srccon)) == -1) {
        virReportSystemError(errno,
                             _("unable to set security context range '%s'"), dst);
        goto cleanup;
    }

    if (!(str = context_str(dstcon))) {
        virReportSystemError(errno, "%s",
                             _("Unable to format SELinux context"));
        goto cleanup;
    }

    ret = g_strdup(str);

 cleanup:
    if (srccon) context_free(srccon);
    if (dstcon) context_free(dstcon);
    return ret;
}