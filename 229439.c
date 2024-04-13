virSecuritySELinuxRestoreSecurityChardevCallback(virDomainDef *def,
                                                 virDomainChrDef *dev G_GNUC_UNUSED,
                                                 void *opaque)
{
    struct _virSecuritySELinuxChardevCallbackData *data = opaque;

    return virSecuritySELinuxRestoreChardevLabel(data->mgr, def, dev->source,
                                                 data->chardevStdioLogd);
}