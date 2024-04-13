virSecuritySELinuxGetBaseLabel(virSecurityManager *mgr, int virtType)
{
    virSecuritySELinuxData *priv = virSecurityManagerGetPrivateData(mgr);
    if (virtType == VIR_DOMAIN_VIRT_QEMU && priv->alt_domain_context)
        return priv->alt_domain_context;
    else
        return priv->domain_context;
}