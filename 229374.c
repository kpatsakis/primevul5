virSecuritySELinuxSetSecurityChardevCallback(virDomainDef *def,
                                             virDomainChrDef *dev G_GNUC_UNUSED,
                                             void *opaque)
{
    struct _virSecuritySELinuxChardevCallbackData *data = opaque;

    return virSecuritySELinuxSetChardevLabel(data->mgr, def, dev->source,
                                             data->chardevStdioLogd);
}