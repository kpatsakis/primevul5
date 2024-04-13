virSecuritySELinuxRestoreSCSILabel(virSCSIDevice *dev,
                                   const char *file,
                                   void *opaque)
{
    virSecurityManager *mgr = opaque;

    /* Don't restore labels on a shareable or readonly hostdev, because
     * other VMs may still be accessing.
     */
    if (virSCSIDeviceGetShareable(dev) || virSCSIDeviceGetReadonly(dev))
        return 0;

    return virSecuritySELinuxRestoreFileLabel(mgr, file, true);
}