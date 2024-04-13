virSecuritySELinuxRememberLabel(const char *path,
                                const char *con)
{
    return virSecuritySetRememberedLabel(SECURITY_SELINUX_NAME,
                                         path, con);
}