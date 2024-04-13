virSecuritySELinuxMCSRemove(virSecurityManager *mgr,
                            const char *mcs)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    virHashRemoveEntry(data->mcs, mcs);
}