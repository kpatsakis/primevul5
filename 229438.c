virSecuritySELinuxRestoreHostLabel(virSCSIVHostDevice *dev G_GNUC_UNUSED,
                                   const char *file,
                                   void *opaque)
{
    virSecurityManager *mgr = opaque;

    return virSecuritySELinuxRestoreFileLabel(mgr, file, true);
}