virSecuritySELinuxMCSAdd(virSecurityManager *mgr,
                         const char *mcs)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    if (virHashLookup(data->mcs, mcs))
        return 1;

    if (virHashAddEntry(data->mcs, mcs, (void*)0x1) < 0)
        return -1;

    return 0;
}