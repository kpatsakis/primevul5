getContext(virSecurityManager *mgr G_GNUC_UNUSED,
           const char *newpath, mode_t mode, char **fcon)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    return selabel_lookup_raw(data->label_handle, fcon, newpath, mode);
}