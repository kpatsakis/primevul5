virSecuritySELinuxSetSCSILabel(virSCSIDevice *dev,
                               const char *file, void *opaque)
{
    virSecurityLabelDef *secdef;
    virSecuritySELinuxCallbackData *ptr = opaque;
    virSecurityManager *mgr = ptr->mgr;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    secdef = virDomainDefGetSecurityLabelDef(ptr->def, SECURITY_SELINUX_NAME);
    if (secdef == NULL)
        return 0;

    if (virSCSIDeviceGetShareable(dev))
        return virSecuritySELinuxSetFilecon(mgr, file,
                                            data->file_context, true);
    else if (virSCSIDeviceGetReadonly(dev))
        return virSecuritySELinuxSetFilecon(mgr, file,
                                            data->content_context, true);
    else
        return virSecuritySELinuxSetFilecon(mgr, file,
                                            secdef->imagelabel, true);
}