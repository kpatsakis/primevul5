virSecuritySELinuxSetHostdevLabelHelper(const char *file,
                                        bool remember,
                                        void *opaque)
{
    virSecurityLabelDef *secdef;
    virSecuritySELinuxCallbackData *data = opaque;
    virSecurityManager *mgr = data->mgr;
    virDomainDef *def = data->def;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (secdef == NULL)
        return 0;
    return virSecuritySELinuxSetFilecon(mgr, file, secdef->imagelabel, remember);
}