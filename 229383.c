virSecuritySELinuxInitialize(virSecurityManager *mgr)
{
    VIR_DEBUG("SELinuxInitialize %s", virSecurityManagerGetVirtDriver(mgr));

    if (virThreadLocalInit(&contextList,
                           virSecuritySELinuxContextListFree) < 0) {
        virReportSystemError(errno, "%s",
                             _("Unable to initialize thread local variable"));
        return -1;
    }

    if (STREQ(virSecurityManagerGetVirtDriver(mgr), "LXC")) {
        return virSecuritySELinuxLXCInitialize(mgr);
    } else {
        return virSecuritySELinuxQEMUInitialize(mgr);
    }
}