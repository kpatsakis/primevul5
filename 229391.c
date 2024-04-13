virSecuritySELinuxRestoreSecuritySmartcardCallback(virDomainDef *def,
                                                   virDomainSmartcardDef *dev,
                                                   void *opaque)
{
    virSecurityManager *mgr = opaque;
    const char *database;

    switch (dev->type) {
    case VIR_DOMAIN_SMARTCARD_TYPE_HOST:
        break;

    case VIR_DOMAIN_SMARTCARD_TYPE_HOST_CERTIFICATES:
        database = dev->data.cert.database;
        if (!database)
            database = VIR_DOMAIN_SMARTCARD_DEFAULT_DATABASE;
        return virSecuritySELinuxRestoreFileLabel(mgr, database, true);

    case VIR_DOMAIN_SMARTCARD_TYPE_PASSTHROUGH:
        return virSecuritySELinuxRestoreChardevLabel(mgr, def,
                                                     dev->data.passthru, false);

    default:
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("unknown smartcard type %d"),
                       dev->type);
        return -1;
    }

    return 0;
}