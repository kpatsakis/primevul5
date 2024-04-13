virSecuritySELinuxMoveImageMetadataHelper(pid_t pid G_GNUC_UNUSED,
                                          void *opaque)
{
    struct virSecuritySELinuxMoveImageMetadataData *data = opaque;
    const char *paths[2] = { data->src, data->dst };
    virSecurityManagerMetadataLockState *state;
    int ret;

    if (!(state = virSecurityManagerMetadataLock(data->mgr, paths, G_N_ELEMENTS(paths))))
        return -1;

    ret = virSecurityMoveRememberedLabel(SECURITY_SELINUX_NAME, data->src, data->dst);
    virSecurityManagerMetadataUnlock(data->mgr, &state);

    if (ret == -2) {
        /* Libvirt built without XATTRS */
        ret = 0;
    }

    return ret;
}