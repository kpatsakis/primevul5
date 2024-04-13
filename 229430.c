virSecuritySELinuxSetPCILabel(virPCIDevice *dev G_GNUC_UNUSED,
                              const char *file, void *opaque)
{
    return virSecuritySELinuxSetHostdevLabelHelper(file, true, opaque);
}