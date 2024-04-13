virSecuritySELinuxSetSecuritySmartcardCallback(virDomainDef *def,
                                               virDomainSmartcardDef *dev,
                                               void *opaque)
{
    const char *database;
    virSecurityManager *mgr = opaque;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    switch (dev->type) {
    case VIR_DOMAIN_SMARTCARD_TYPE_HOST:
        break;

    case VIR_DOMAIN_SMARTCARD_TYPE_HOST_CERTIFICATES:
        database = dev->data.cert.database;
        if (!database)
            database = VIR_DOMAIN_SMARTCARD_DEFAULT_DATABASE;
        return virSecuritySELinuxSetFilecon(mgr, database, data->content_context, true);

    case VIR_DOMAIN_SMARTCARD_TYPE_PASSTHROUGH:
        return virSecuritySELinuxSetChardevLabel(mgr, def,
                                                 dev->data.passthru, false);

    default:
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("unknown smartcard type %d"),
                       dev->type);
        return -1;
    }

    return 0;
}