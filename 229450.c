virSecuritySELinuxDriverOpen(virSecurityManager *mgr)
{
    return virSecuritySELinuxInitialize(mgr);
}