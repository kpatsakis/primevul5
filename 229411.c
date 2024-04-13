virSecuritySELinuxDriverProbe(const char *virtDriver)
{
    if (is_selinux_enabled() <= 0)
        return SECURITY_DRIVER_DISABLE;

    if (virtDriver && STREQ(virtDriver, "LXC") &&
        !virFileExists(selinux_lxc_contexts_path())) {
        return SECURITY_DRIVER_DISABLE;
    }

    return SECURITY_DRIVER_ENABLE;
}