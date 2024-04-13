virSecuritySELinuxSetTapFDLabel(virSecurityManager *mgr,
                                virDomainDef *def,
                                int fd)
{
    struct stat buf;
    char *fcon = NULL;
    virSecurityLabelDef *secdef;
    char *str = NULL, *proc = NULL, *fd_path = NULL;
    int rc = -1;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->label)
        return 0;

    if (fstat(fd, &buf) < 0) {
        virReportSystemError(errno, _("cannot stat tap fd %d"), fd);
        goto cleanup;
    }

    if ((buf.st_mode & S_IFMT) != S_IFCHR) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("tap fd %d is not character device"), fd);
        goto cleanup;
    }

    /* Label /dev/tap([0-9]+)? devices only. Leave /dev/net/tun alone! */
    proc = g_strdup_printf("/proc/self/fd/%d", fd);

    if (virFileResolveLink(proc, &fd_path) < 0) {
        virReportSystemError(errno,
                             _("Unable to resolve link: %s"), proc);
        goto cleanup;
    }

    if (!STRPREFIX(fd_path, "/dev/tap")) {
        VIR_DEBUG("fd=%d points to %s not setting SELinux label",
                  fd, fd_path);
        rc = 0;
        goto cleanup;
    }

    if (getContext(mgr, fd_path, buf.st_mode, &fcon) < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("cannot lookup default selinux label for tap fd %d"), fd);
        goto cleanup;
    }

    if (!(str = virSecuritySELinuxContextAddRange(secdef->label, fcon))) {
        goto cleanup;
    } else {
        rc = virSecuritySELinuxFSetFilecon(fd, str);
    }

 cleanup:
    freecon(fcon);
    VIR_FREE(fd_path);
    VIR_FREE(proc);
    VIR_FREE(str);
    return rc;
}