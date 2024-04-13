virSecuritySELinuxRecallLabel(const char *path,
                              char **con)
{
    int rv;

    rv = virSecurityGetRememberedLabel(SECURITY_SELINUX_NAME, path, con);
    if (rv < 0)
        return rv;

    if (!*con)
        return 1;

    return 0;
}